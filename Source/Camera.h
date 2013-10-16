#pragma once

#include "Node.h"

#include "cinder/Rect.h"
#include "cinder/Ray.h"

namespace cinder { class CameraPersp; }

class Camera : public Node {
public:
	static Camera* get();
	virtual ~Camera();
	virtual void update( const float deltaTime = 0.0f );
	
	ci::CameraPersp& cinderCamera() { return *mCinderCamera; }
	virtual ci::Vec3f getGlobalPosition();
	
	void setScreenSize( int width, int height, float contentScaleFactor );
	void setRange( float near, float far );
	
	void setFov( float fov );
	
	void setZoom( float zoom );
	float getZoom() const { return mBody.position.z; }
	
	const Node& getBody() const { return mBody; }
	
	float getAngle() const { return mPivot.rotation.x; }
	void setAngle( const float angleX ) { mPivot.rotation.x = angleX; }
	
	ci::Ray rayIntoScene( ci::Vec2i screenPoint );
	
	ci::Matrix44f getModelViewMatrix() const;
	ci::Matrix44f getProjectionMatrix() const;
	const ci::Vec3f& getDirection() const;
	
	ci::Vec2i getScreenSize() const { return mScreenSize; }
	ci::Vec2i getScaledScreenSize() const { return mScreenSize * mContentScaleFactor; }
	float getScaledScreenWidth() const { return mScreenSize.x * mContentScaleFactor; }
	float getScaledScreenHeight() const { return mScreenSize.y * mContentScaleFactor; }
	ci::Vec2i getScreenCenter() const { return mScreenSize / 2; }
	ci::Vec2f getWorldToScreen( const ci::Vec3f &worldCoord ) const;
	ci::Rectf getScreenRect() const { return mScreenRect; }
	
	const float getFarPlaneDistance() const { return mFar; }
	const float getNearPlaneDistance() const { return mNear; }
	
private:
	static Camera* sInstance;
	Camera();
	
	float mContentScaleFactor;
	float mFov;
	Node mBody;
	float mNear;
	float mFar;
	Node mPivot;
	ci::Vec2i mScreenSize;
	float mScreenAspectRatio;
	ci::Rectf mScreenRect;
	ci::Vec3f mLookDirection;
	ci::CameraPersp* mCinderCamera;
	
};