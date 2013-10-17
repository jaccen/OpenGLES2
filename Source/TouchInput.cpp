#include "TouchInput.h"

using namespace ci;

TouchInput* TouchInput::sInstance = NULL;

TouchInput* TouchInput::get()
{
	if ( sInstance == NULL ) {
		sInstance = new TouchInput();
	}
	return sInstance;
}

TouchInput::TouchInput() : mPrevTouchCount(0), mTouchCount(0), mTouchDistanceStart(0.0f), mTouchDistanceCurrent(0.0f)
{
	mTouchCenter = Vec2i(0,0);
	mTouchCenterStart = Vec2i(0,0);
	mSingleTapTimer = ly::Timer( boost::bind( &TouchInput::onSingleTapTimerExpired, this, boost::arg<1>() ), 0.05f, 1 );
}

TouchInput::~TouchInput() {}

void TouchInput::addDelegate( IDelegate* delegate )
{
	// Check for a duplicate, then add
	std::list<IDelegate*>::const_iterator existing = std::find( mDelegates.begin(), mDelegates.end(), delegate );
	if ( existing == mDelegates.end() ) {
		mDelegates.push_back( delegate );
	}
}

void TouchInput::removeDelegate( IDelegate* delegate )
{
	// Check for existence, then erase
	std::list<IDelegate*>::iterator existing = std::find( mDelegates.begin(), mDelegates.end(), delegate );
	if ( existing != mDelegates.end() ) {
		mDelegates.erase( existing );
	}
}

void TouchInput::removeAllDelegates()
{
	mDelegates.clear();
}

Vec2f TouchInput::getTouchesCenter( const std::vector<ci::Vec2i>& touches)
{
	Vec2f total = Vec2f::zero();
	for( std::vector<ci::Vec2i>::const_iterator iter = touches.begin(); iter != touches.end(); iter++ )
		total = total + *iter;
	Vec2f output = total / (float) touches.size();
	return output;
}

float TouchInput::getDistance( const std::vector<ci::Vec2i>& touches)
{
	float greatestDistance = 0.0f;
	for( std::vector<ci::Vec2i>::const_iterator iter_a = touches.begin(); iter_a != touches.end(); iter_a++ ) {
		for( std::vector<ci::Vec2i>::const_iterator iter_b = touches.begin(); iter_b != touches.end(); iter_b++ ) {
			float distance = iter_b->distance( *iter_a );
			if ( distance > greatestDistance ) {
				greatestDistance = distance;
			}
		}
	}
	return greatestDistance;
}

ci::Vec2f TouchInput::getScale( ci::Matrix44f& matrix )
{
	Vec2f scale = Vec2f::zero();
	Vec3f columns[2] = {
		matrix.getColumn(0).xyz(),
		matrix.getColumn(1).xyz()
	};
	scale.x = columns[0].length();
	scale.y = columns[1].length();
	return scale;
}

void TouchInput::update( const float deltaTime )
{
	bool didChange = mPrevTouchCount != mTouchCount;
	
	if ( didChange ) {
		mTouchCenter = mTouchCenterStart = getTouchesCenter( mTouches );
		mTouchDistanceStart = mTouchDistanceCurrent = getDistance( mTouches );
	}
	else {
		mTouchCenter = getTouchesCenter( mTouches );
		mTouchDistanceCurrent = TouchInput::getDistance( mTouches );
	}
	
	if ( !mCurrentGesture.isActive && mTouchCount > 0 ) {
		resetCurrentGesture();
		mCurrentGesture.isActive = true;
		mCurrentGesture.touchCount = mTouchCount;
		mCurrentGesture.isTap = false;
		std::cout << "GESTURE START: " << mCurrentGesture.touchCount << std::endl;
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureStarted( mCurrentGesture.touchCount );
		}
	}
	
	if ( !mCurrentGesture.isTap && mCurrentGesture.isActive ) {
		std::cout << "GESTURE MOVED: " << mCurrentGesture.touchCount << std::endl;
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureMoved( mCurrentGesture.touchCount );
		}
	}
	
	mPrevTouchCount = mTouchCount;
	
	mSingleTapTimer.update( deltaTime );
}

void TouchInput::touchesEnded( const std::vector<ci::Vec2i>& positions )
{
	mPrevTouchCount = getTouchCount();
	for( int i = 0; i < positions.size(); i++ ) {
		if ( !mTouches.empty() ) {
			mTouches.erase( mTouches.begin() );
		}
	}
	mTouchCount = mTouches.size();
	
	if ( mCurrentGesture.isActive && mCurrentGesture.isTap && mCurrentGesture.touchCount == 1 && mTouchCount == 0 ) {
		onSingleTapTimerExpired( 0.0f );
	}
	
	const bool gestureShouldAbort = mCurrentGesture.isActive && mTouchCount != mCurrentGesture.touchCount;
	const bool gestureIsComplete = mCurrentGesture.isActive && mCurrentGesture.isComplete;
	if ( gestureShouldAbort || gestureIsComplete ) {
		resetCurrentGesture();
	}
}

void TouchInput::touchesBegan( const std::vector<ci::Vec2i>& positions )
{
	mPrevTouchCount = getTouchCount();
	mTouches = positions;
	mTouchCount = mTouches.size();
	
	if ( mTouchCount == 1 ) {
		mCurrentGesture.isActive = true;
		mCurrentGesture.touchCount = 1;
		mCurrentGesture.isTap = true;
		mSingleTapPosition = positions[0];
		mSingleTapTimer.start( true );
	}
	else if ( mTouchCount > 1 && mSingleTapTimer.getIsActive() ) {
		onSingleTapTimerExpired( 0.0f );
	}
}

void TouchInput::touchesMoved( const std::vector<ci::Vec2i>& positions )
{
	mPrevTouchCount = getTouchCount();
	mTouches = positions;
	mTouchCount = mTouches.size();
}

void TouchInput::onSingleTapTimerExpired( const float deltaTime )
{
	if ( mSingleTapTimer.getIsActive() && getTouchesCenter().distance( mSingleTapPosition ) < 7.0f ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->tapDown( mSingleTapPosition );
		}
		std::cout << "TAP DOWN" << std::endl;
	}
	else {
		resetCurrentGesture();
	}
}

void TouchInput::resetCurrentGesture()
{
	if ( !mCurrentGesture.isTap && mCurrentGesture.isActive ) {
		std::cout << "GESTURE END: " << mCurrentGesture.touchCount << std::endl;
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureEnded( mCurrentGesture.touchCount );
		}
	}
	
	mCurrentGesture.isTap = false;
	mCurrentGesture.isActive = false;
	mCurrentGesture.isComplete = false;
	mCurrentGesture.touchCount = 0;
}
