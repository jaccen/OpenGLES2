#include "Touch.h"

using namespace ci;

Touch::Touch() : mPrevTouchCount(0), mTouchCount(0), mTouchDistanceStart(0.0f), mTouchDistanceCurrent(0.0f)
{
	mTouchCenter = Vec2i(0,0);
	mTouchCenterStart = Vec2i(0,0);
}

Touch::~Touch() {}

void Touch::addDelegate( IDelegate* delegate )
{
	// Check for a duplicate, then add
	std::list<IDelegate*>::const_iterator existing = std::find( mDelegates.begin(), mDelegates.end(), delegate );
	if ( existing == mDelegates.end() ) {
		mDelegates.push_back( delegate );
	}
}

void Touch::removeDelegate( IDelegate* delegate )
{
	// Check for existence, then erase
	std::list<IDelegate*>::iterator existing = std::find( mDelegates.begin(), mDelegates.end(), delegate );
	if ( existing != mDelegates.end() ) {
		mDelegates.erase( existing );
	}
}

void Touch::removeAllDelegates()
{
	mDelegates.clear();
}

Vec2f Touch::getTouchesCenter( const std::vector<ci::Vec2i>& touches)
{
	Vec2f total = Vec2f::zero();
	for( std::vector<ci::Vec2i>::const_iterator iter = touches.begin(); iter != touches.end(); iter++ )
		total = total + *iter;
	Vec2f output = total / (float) touches.size();
	return output;
}

float Touch::getDistance( const std::vector<ci::Vec2i>& touches)
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

ci::Vec2f Touch::getScale( ci::Matrix44f& matrix )
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

void Touch::update( const float deltaTime )
{
	//std::cout << "mTouchCount = " << mTouchCount;
	//std::cout << ", mPrevTouchCount = " << mPrevTouchCount << std::endl;
	bool didChange = mPrevTouchCount != mTouchCount;
	
	if ( didChange ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->gestureEnded( mPrevTouchCount );
			(*iter)->gestureStarted( mTouchCount );
		}
	}
	
	if ( mTouchCount == 2 ) {
		if ( didChange ) {
			mTouchCenter = mTouchCenterStart = getTouchesCenter( mTouches );
			mTouchDistanceStart = mTouchDistanceCurrent = getDistance( mTouches );
		}
		else {
			mTouchCenter = getTouchesCenter( mTouches );
			mTouchDistanceCurrent = Touch::getDistance( mTouches );
		}
	}
	
	mPrevTouchCount = mTouchCount;
}

void Touch::touchesEnded( const std::vector<ci::Vec2i>& positions )
{
	mTouchCount = positions.size()-1;
}

void Touch::touchesBegan( const std::vector<ci::Vec2i>& positions )
{
	if ( positions.size() == 1 ) {
		for( std::list<IDelegate*>::const_iterator iter = mDelegates.begin(); iter != mDelegates.end(); iter++ ) {
			(*iter)->tapDown( positions[0] );
		}
	}
}

void Touch::touchesMoved( const std::vector<ci::Vec2i>& positions )
{
	mTouches = positions;
	mTouchCount = positions.size();
}