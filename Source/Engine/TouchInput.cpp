#include "TouchInput.h"
#include "Game.h"

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
	mAcceleration = Vec3f::zero();
	
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

void TouchInput::setAcceleration( const float x, const float y, const float z )
{
	mAcceleration = Vec3f( x, y, z );
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
		//std::cout << "GESTURE START: " << mCurrentGesture.touchCount << std::endl;
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureStarted( mCurrentGesture.touchCount );
		}
	}
	
	if ( !mCurrentGesture.isTap && mCurrentGesture.isActive ) {
		//std::cout << "GESTURE MOVED: " << mCurrentGesture.touchCount << std::endl;
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureMoved( mCurrentGesture.touchCount );
		}
	}
	
	mPrevTouchCount = mTouchCount;
	
	mSingleTapTimer.update( deltaTime );
}

void TouchInput::touchesEnded( const std::vector<ci::Vec2i>& positions )
{
	if ( Node2d* node = testScreenNodeTouches( positions ) ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			node->setCurrentState( Node2d::State::NORMAL );
			(*iter)->nodeTouchUp( node );
		}
		if ( node->swallowsTouches ) {
			return;
		}
	}
	
	if ( mTouchCount == 1 ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->tapUp( positions[0] );
		}
	}
	
	mPrevTouchCount = getTouchCount();
	for( int i = 0; i < positions.size(); i++ ) {
		if ( !mTouches.empty() ) {
			mTouches.erase( mTouches.begin() );
		}
	}
	mTouchCount = math<int>::max( mTouchCount - positions.size(), 0 );
	
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
	if ( Node2d* node = testScreenNodeTouches( positions ) ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			node->setCurrentState( Node2d::State::HIGHLIGHTED );
			(*iter)->nodeTouchDown( node );
		}
		if ( node->swallowsTouches ) {
			return;
		}
	}
	
	mPrevTouchCount = getTouchCount();
	mTouches = positions;
	mTouchCount = positions.size();
	
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
	mTouchCount = positions.size();
}

Node2d* TouchInput::testScreenNodeTouches( const std::vector<ci::Vec2i>& positions )
{
	for( auto scene : Game::get()->getScenes() ) {
		for( auto node : scene->getScreenNodes() ) {
			for( auto touchPos : positions ) {
				if ( Area( node->position.x, node->position.y, node->position.x + node->size.x, node->position.y + node->size.y ).contains( touchPos ) ) {
					return node;
				}
			}
		}
	}
	return NULL;
}

void TouchInput::resetCurrentGesture()
{
	if ( !mCurrentGesture.isTap && mCurrentGesture.isActive ) {
		//std::cout << "GESTURE END: " << mCurrentGesture.touchCount << std::endl;
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureEnded( mCurrentGesture.touchCount );
		}
	}
	
	mCurrentGesture.isTap = false;
	mCurrentGesture.isActive = false;
	mCurrentGesture.isComplete = false;
	mCurrentGesture.touchCount = 0;
}

void TouchInput::onSingleTapTimerExpired( const float deltaTime )
{
	if ( mSingleTapTimer.getIsActive() && getTouchesCenter().distance( mSingleTapPosition ) < 7.0f ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->tapDown( mSingleTapPosition );
		}
		//std::cout << "TAP DOWN" << std::endl;
	}
	else {
		resetCurrentGesture();
	}
}