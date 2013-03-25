#pragma once

#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"

#include <vector>
#include <list>

class Touch {
public:
	class IDelegate {
	public:
		// Add (pure) virtual methods here
		virtual void gestureStarted( int fingerCount ) = 0;
		virtual void gestureEnded( int fingerCount ) = 0;
		virtual void tapDown( ci::Vec2i position ) = 0;
		virtual void tapUp( ci::Vec2i position ) = 0;
	};
	
	Touch();
	~Touch();
	
	void addDelegate( IDelegate* delegate );
	void removeDelegate( IDelegate* delegate );
	void removeAllDelegates();
	
	static ci::Vec2f	getTouchesCenter( const std::vector<ci::Vec2i>& touches);
	static float		getDistance( const std::vector<ci::Vec2i>& touches);
	static ci::Vec2f	getScale( ci::Matrix44f& matrix );
	
    void				touchesEnded( const std::vector<ci::Vec2i>& positions );
    void				touchesBegan( const std::vector<ci::Vec2i>& positions );
    void				touchesMoved( const std::vector<ci::Vec2i>& positions );
	void				update( const float deltaTime );
	
	ci::Vec2i			getTouchesCenter() const { return mTouchCenter; }
	float				getTouchesDistance() const { return mTouchDistanceCurrent - mTouchDistanceStart; }
	ci::Vec2i			getTouchesDifference() const { return mTouchCenter - mTouchCenterStart; }
	
private:
	std::vector<ci::Vec2i> mTouches;
	ci::Vec2i			mTouchCenter;
	ci::Vec2i			mTouchCenterStart;
	int					mTouchCount;
	int					mPrevTouchCount;
	
	float				mTouchDistanceStart;
	float				mTouchDistanceCurrent;
	
	std::list<IDelegate*> mDelegates;
};