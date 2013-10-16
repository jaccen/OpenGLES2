#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace ly {

static int timerIdCounter = 0;

class Timer {
public:
	Timer( boost::function<void(const float)> callback, const float limit, const int loops = 1 ) : mCurrentTime(0.0f), mCallback( callback ), mLimit( limit ), mLoops( loops ), mActive( false )
	{
		mTimerId = timerIdCounter++;
	}
	
	Timer() {}
	virtual ~Timer() {}
	
	void start( bool andReset = false, bool asExpired = false )
	{
		mActive = true;
		if( andReset ) reset();
		else if ( asExpired ) mCurrentTime = mLimit;
	}
	
	void stop( bool andReset = false )
	{
		mActive = false;
		if( andReset ) reset();
	}
	
	void toggle() { if ( mActive ) stop(); else start(); }
	
	void reset() { mCurrentTime = 0; }
	
	void update( const float deltaTime )
	{
		if ( mActive ) {
			mCurrentTime += deltaTime;
			if ( mCurrentTime >= mLimit ) {
				mLoopsPassed++;
				mCallback( mCurrentTime );
				mCurrentTime = 0;
				if ( mLoopsPassed >= mLoops && mLoops > 0 ) {
					mActive = false;
					mExpired = true;
				}
			}
		}
	}
	
	bool loops() const { return mLoops; }
	float currentTime() const { return mCurrentTime; }
	float limit() const { return mLimit; }
	void setLimit( const float limit ) { mLimit = limit; }
	void setLoops( const bool loops ) { mLoops = loops; }
	bool operator==( Timer* timer ) { return timer->timerId() == mTimerId; }
	bool expired() const { return mExpired; }
	int timerId() const { return mTimerId; }
	
private:

	boost::function<void (const float)> mCallback;
	
	float mCurrentTime;
	float mLimit;
	int mLoops;
	int mTimerId;
	int mLoopsPassed;
	bool mExpired;
	bool mActive;
};
	
}