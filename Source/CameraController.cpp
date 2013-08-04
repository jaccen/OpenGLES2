#include "CameraController.h"

using namespace ci;

EditorCamera::EditorCamera() : CameraController() {}

EditorCamera::EditorCamera( Camera* camera ) : CameraController()
{
	mCamera = camera;
	
	mCamera->setZoom( 400.0f );
	mCamera->setFov( 60.0f );
	mCamera->setAngle( 25.0f );
	mCamera->rotation.y = -85;
	mZoomStart = mCamera->getZoom();
	mRotationStart.x = mCamera->getAngle();
	mRotationStart.y = mCamera->rotation.y;
	
	mOrbitSpeed = Vec2f( 0.5f, 0.5f );
	mZoomSpeed = 2.5f;
	mPanSpeed = 0.2f;
}

void EditorCamera::gestureStarted( int fingerCount ) {}

void EditorCamera::gestureEnded( int fingerCount ) {}

void EditorCamera::update( const float deltaTime )
{
	TouchInput* touch = TouchInput::get();
	Vec2f diff = touch->getTouchesDifference();
	
	if ( touch->getTouchCount() > 1  ) {
		Vec2f target = Vec2f(mRotationStart.x - diff.y * mOrbitSpeed.y,
							 mRotationStart.y - diff.x * mOrbitSpeed.x );
		mCamera->rotation.y += (target.y - mCamera->rotation.y ) / 10.0f;
		mCamera->rotation.x += (target.x - mCamera->rotation.x ) / 10.0f;
		
		float zoomTarget = mZoomStart - touch->getTouchesDistance() * mZoomSpeed;
		float easedZoom = ( zoomTarget - mCamera->getZoom() ) / 10.0f;
		mCamera->setZoom( zoomTarget );
	}
	else {
		mRotationStart.x = mCamera->rotation.x;
		mRotationStart.y = mCamera->rotation.y;
		
		mZoomStart = mCamera->getZoom();
	}
}

FirstPersonCamera::FirstPersonCamera( Camera* camera )
{
	mCamera = camera;
	mCamera->setZoom( 0 );
	//mCamera->setPivotHeight( 0.5f );
	mCamera->position.z = 10.0f;
	mCamera->rotation.y = 0.0f;
	
	mMoveSpeed = 0.3f;
	mStrafeSpeed = 0.2f;
	mLookSensitivity = 0.4f;
}

void FirstPersonCamera::update( const float deltaTime )
{
	/*Input* input = Input::get();
	
	mCamera->rotation.y -= input->moveVelocity().x * mLookSensitivity;
	mCamera->mPivot.rotation.x -= input->moveVelocity().y * mLookSensitivity;*/
}


