#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"

#include "Timer.h"

#include <vector>
#include <list>

class TouchInput {
public:
	struct Gesture {
		Gesture() : isActive( false ), touchCount(0), isTap( false ), isComplete(false) {}
		bool isActive;
		bool isTap;
		bool isComplete;
		int touchCount;
	};
	
	class IDelegate {
	public:
		// Add (pure) virtual methods here
		virtual void gestureStarted( int fingerCount ) = 0;
		virtual void gestureMoved( int fingerCount ) = 0;
		virtual void gestureEnded( int fingerCount ) = 0;
		virtual void tapDown( ci::Vec2i position ) = 0;
		virtual void tapUp( ci::Vec2i position ) = 0;
	};
	
	class FindTouch {
	public:
		FindTouch( ci::Vec2i pos ) { mPos = pos; }
		bool operator()( ci::Vec2i pos ) const { return mPos.distance( pos ) < 20.0f; }
	private:
		ci::Vec2i mPos;
	};
	
	static TouchInput*	get();
	
	void				addDelegate( IDelegate* delegate );
	void				removeDelegate( IDelegate* delegate );
	void				removeAllDelegates();
	
	static ci::Vec2f	getTouchesCenter( const std::vector<ci::Vec2i>& touches);
	static float		getDistance( const std::vector<ci::Vec2i>& touches);
	static ci::Vec2f	getScale( ci::Matrix44f& matrix );
	
    void				touchesEnded( const std::vector<ci::Vec2i>& positions );
    void				touchesBegan( const std::vector<ci::Vec2i>& positions );
    void				touchesMoved( const std::vector<ci::Vec2i>& positions );
	
	void				update( const float deltaTime );
	void				onSingleTapTimerExpired( const float deltaTime );
	
	ci::Vec2i			getTouchesCenter() const { return mTouchCenter; }
	float				getTouchesDistance() const { return mTouchDistanceCurrent - mTouchDistanceStart; }
	ci::Vec2i			getTouchesDifference() const { return mTouchCenter - mTouchCenterStart; }
	
	int					getTouchCount() const { return mTouchCount; }
	
private:
	TouchInput();
	~TouchInput();
	static TouchInput* sInstance;
	
	Gesture				mCurrentGesture;
	inline void			resetCurrentGesture();
	
	std::vector<ci::Vec2i> mTouches;
	ci::Vec2i			mTouchCenter;
	ci::Vec2i			mTouchCenterStart;
	int					mTouchCount;
	int					mPrevTouchCount;
	
	float				mTouchDistanceStart;
	float				mTouchDistanceCurrent;
	
	std::list<IDelegate*> mDelegates;
	
	ly::Timer			mSingleTapTimer;
	ci::Vec2i			mSingleTapPosition;
};