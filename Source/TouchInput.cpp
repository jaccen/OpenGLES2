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
	
	if ( didChange && mTouchCount > 0 ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureEnded( mPrevTouchCount );
			(*iter)->gestureStarted( mTouchCount );
			mGestureActive = true;
		}
	}
	else if ( mTouchCount == 0 ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureEnded( mPrevTouchCount );
			mGestureActive = false;
		}
	}
	
	if ( mGestureActive ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureMoved( mTouchCount );
		}
	}
	
	mPrevTouchCount = mTouchCount;
}

void TouchInput::touchesEnded( const std::vector<ci::Vec2i>& positions )
{
	if ( mTouchCount == 1 ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->tapUp( positions[0] );
		}
	}
	
	mTouchCount = math<int>::max( mTouchCount - positions.size(), 0 );
}

void TouchInput::touchesBegan( const std::vector<ci::Vec2i>& positions )
{
	mTouches = positions;
	mTouchCount = positions.size();
	
	if ( mTouchCount == 1 ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->tapDown( positions[0] );
		}
	}
}

void TouchInput::touchesMoved( const std::vector<ci::Vec2i>& positions )
{
	mTouches = positions;
	mTouchCount = positions.size();
}

