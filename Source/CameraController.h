#pragma once

#include "Camera.h"
#include "TouchInput.h"

#include "cinder/Vector.h"

class CameraController : TouchInput::IDelegate {
public:
	CameraController() : mCamera( NULL ) {}
	virtual void update( const float deltaTime ) = 0;
protected:
	Camera* mCamera;
	
	virtual void gestureStarted( int fingerCount ) {}
	virtual void gestureEnded( int fingerCount ) {}
	virtual void tapDown(ci::Vec2i position) {}
	virtual void tapUp(ci::Vec2i position) {}
};

class EditorCamera : public CameraController {
public:
	EditorCamera();
	EditorCamera( Camera* camera );
	virtual void update( const float deltaTime );
	
	virtual void gestureStarted( int fingerCount );
	virtual void gestureEnded( int fingerCount );
	virtual void tapDown(ci::Vec2i position) {}
	virtual void tapUp(ci::Vec2i position) {}
private:
	ci::Vec2f mRotationStart;
	float mZoomStart;
	float mZoomSpeed;
	ci::Vec2f mOrbitSpeed;
	float mPanSpeed;
};

class FirstPersonCamera : public CameraController {
public:
	FirstPersonCamera( Camera* camera);
	virtual void update( const float deltaTime );
private:
	float mLookSensitivity;
	float mStrafeSpeed;
	float mMoveSpeed;
};