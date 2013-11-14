#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"

#include "Timer.h"
#include "Node2d.h"

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
		virtual void nodeTouchDown( Node2d* node ) {};
		virtual void nodeTouchMove( Node2d* node ) {};
		virtual void nodeTouchUp( Node2d* node ) {};
		
		virtual void gestureStarted( int fingerCount ) {};
		virtual void gestureMoved( int fingerCount ) {};
		virtual void gestureEnded( int fingerCount ) {};
		
		virtual void tapDown( ci::Vec2i position ) {};
		virtual void tapUp( ci::Vec2i position ) {};
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
	
	void				setAcceleration( const float x, const float y, const float z );
	const ci::Vec3f&	getAcceleration() const { return mAcceleration; }
	
	void				update( const float deltaTime );
	
	ci::Vec2i			getTouchesCenter() const { return mTouchCenter; }
	float				getTouchesDistance() const { return mTouchDistanceCurrent - mTouchDistanceStart; }
	ci::Vec2i			getTouchesDifference() const { return mTouchCenter - mTouchCenterStart; }
	
	int					getTouchCount() const { return mTouchCount; }
	
private:
	TouchInput();
	~TouchInput();
	static TouchInput* sInstance;
	
	inline Node2d*		testScreenNodeTouches( const std::vector<ci::Vec2i>& positions );
	
	ci::Vec3f			mAcceleration;
	
	std::vector<ci::Vec2i> mTouches;
	ci::Vec2i			mTouchCenter;
	ci::Vec2i			mTouchCenterStart;
	int					mTouchCount;
	int					mPrevTouchCount;
	
	float				mTouchDistanceStart;
	float				mTouchDistanceCurrent;
	
	std::list<IDelegate*> mDelegates;
	
	Gesture				mCurrentGesture;
	inline void			resetCurrentGesture();
	ly::Timer			mSingleTapTimer;
	ci::Vec2i			mSingleTapPosition;
	void				onSingleTapTimerExpired( const float deltaTime );
};