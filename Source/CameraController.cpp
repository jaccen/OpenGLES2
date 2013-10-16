#include "CameraController.h"

using namespace ci;

EditorCamera::EditorCamera() : CameraController() {}

EditorCamera::EditorCamera( Camera* camera ) : CameraController()
{
	mCamera = camera;
	
	mZoomStart = mCamera->getZoom();
	mRotationStart.x = mCamera->getAngle();
	mRotationStart.y = mCamera->rotation.y;
	mPositionStart = mCamera->position;
	
	mOrbitSpeed = Vec2f( 0.5f, 0.5f );
	mZoomSpeed = 10.5f;
	mPanSpeed = 0.2f;
	mMoveSpeed = 0.003f;
}

void EditorCamera::gestureStarted( int fingerCount ) {}

void EditorCamera::gestureEnded( int fingerCount ) {}

void EditorCamera::update( const float deltaTime )
{
	TouchInput* touch = TouchInput::get();
	
	// Two-finger controls for rotate, zoom
	if ( touch->getTouchCount() > 1  ) {
		Vec2f diff = touch->getTouchesDifference();
		Vec2f target = Vec2f(mRotationStart.x - diff.y * mOrbitSpeed.y,
							 mRotationStart.y - diff.x * mOrbitSpeed.x );
		mCamera->rotation.y += (target.y - mCamera->rotation.y ) / 10.0f;
		mCamera->rotation.x += (target.x - mCamera->rotation.x ) / 10.0f;
		
		float zoomTarget = mZoomStart - touch->getTouchesDistance() * mZoomSpeed;
		if ( zoomTarget <= 5000.0f && zoomTarget >= 80.0f ) {
			//mCamera->setZoom( mCamera->getZoom() + (zoomTarget - mCamera->getZoom() ) / 20.0f );
			mCamera->setZoom( zoomTarget );
		}
	}
	else {
		mRotationStart.x = mCamera->rotation.x;
		mRotationStart.y = mCamera->rotation.y;
		
		mZoomStart = mCamera->getZoom();
	}
	
	// One-finger control for panning
	/*if ( touch->getTouchCount() == 1  ) {
		const Vec3f direction = Vec3f( touch->getTouchesDifference().x, 0.0f, touch->getTouchesDifference().y );
		const Vec3f target = mPositionStart - mCamera->getTransform().transformVec( direction ) * mMoveSpeed * mCamera->getZoom();
		mCamera->position += ( target - mCamera->position ) / 2.0f;
	}
	else {
		mPositionStart = mCamera->position;
	}*/
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


