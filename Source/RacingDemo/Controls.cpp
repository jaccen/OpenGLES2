#include "Controls.h"
#include "Game.h"

using namespace ci;

Controls::Controls() : mCamera( NULL )
{
}

void Controls::setup( GameCamera* camera )
{
	mCamera = camera;
	
	mZoomStart = mCamera->getZoom();
	mRotationStart.x = mCamera->getAngle();
	mRotationStart.y = mCamera->rotation.y;
	mPositionStart = mCamera->position;
	mRotationTarget.x = mAngleEased = mCamera->getAngle();
	mRotationTarget.y = mCamera->rotation.y;
	mZoomTarget = mZoomEased = mCamera->getZoom();
	
	mOrbitSpeed = Vec2f( 0.5f, 0.5f );
	mZoomSpeed = 10.5f;
	mPanSpeed = 0.2f;
	mMoveSpeed = 0.003f;
	
	mRotationEasing = 5.0f;
	mRotationXMin = -60.0f;
	mRotationXMax = -10.0f;
	
	mZoomEasing = 5.0f;
	mZoomMax = 5000.0f;
	mZoomMin = 80.0f;
	
	TouchInput::get()->addDelegate( this );
}

void Controls::update( const float deltaTime )
{
	TouchInput* touch = TouchInput::get();
	
	// Calculate two-finger controls for rotate, zoom
	if ( touch->getTouchCount() == 2  ) {
		const Vec2f diff = touch->getTouchesDifference();
		mRotationTarget = Vec2f( mRotationStart.x - diff.y * mOrbitSpeed.y, mRotationStart.y - diff.x * mOrbitSpeed.x );
		mRotationTarget.x = math<float>::clamp( mRotationTarget.x, mRotationXMin, mRotationXMax );
		
		mZoomTarget = ci::math<float>::clamp( mZoomStart - touch->getTouchesDistance() * mZoomSpeed, mZoomMin, mZoomMax );
	}
	else {
		mRotationStart.x = mCamera->getAngle();
		mRotationStart.y = mCamera->rotation.y;
		
		mZoomStart = mCamera->getZoom();
	}
	
	// Three-finger control for panning
	if ( touch->getTouchCount() == 3  ) {
		const Vec3f direction = Vec3f( touch->getTouchesDifference().x, 0.0f, touch->getTouchesDifference().y );
		const Vec3f target = mPositionStart - mCamera->getTransform().transformVec( direction ) * mMoveSpeed * mCamera->getZoom();
		mCamera->position += ( target - mCamera->position ) / 2.0f;
	}
	else {
		mPositionStart = mCamera->position;
	}
	
	// Apply target values with easing
	mCamera->rotation.y += ( mRotationTarget.y - mCamera->rotation.y ) / mRotationEasing;
	mAngleEased += (mRotationTarget.x - mAngleEased ) / mRotationEasing;
	mCamera->setAngle( mAngleEased );
	mZoomEased += (mZoomTarget - mZoomEased ) / mZoomEasing;
	mCamera->setZoom( mZoomEased );
	
	const ci::Vec3f acceleration = TouchInput::get()->getAcceleration();
	
	/*const float m = 100.0f;
	 ci::Vec3f a = acceleration;
	a.x = math<float>::floor( acceleration.x * m ) / m;
	a.y = math<float>::floor( acceleration.y * m ) / m;
	a.x = math<float>::floor( acceleration.z * m ) / m;
	std::cout << "Acceleration: [ " << a.x << ", " << a.y << ", " << a.z << std::endl;*/
	
	const float targetZ = -acceleration.y * 90.0f;
	mCamera->getBody().rotation.z += (targetZ - mCamera->getBody().rotation.z) / 2.0f;
}

void Controls::tapDown( ci::Vec2i position )
{
}

void Controls::tapUp( ci::Vec2i position )
{
}

void Controls::gestureStarted( int fingerCount )
{
}

void Controls::gestureMoved( int fingerCount )
{
}

void Controls::gestureEnded( int fingerCount )
{
}


