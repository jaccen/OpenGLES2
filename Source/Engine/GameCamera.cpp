#include "GameCamera.h"
#include "GameConstants.h"

#include "cinder/Camera.h"
#include "cinder/Quaternion.h"

GameCamera* GameCamera::sInstance = NULL;

GameCamera* GameCamera::get()
{
	if ( !sInstance ) sInstance = new GameCamera();
	return sInstance;
}

ci::Matrix44f GameCamera::getModelViewMatrix() const
{
	return mCinderCamera->getModelViewMatrix();
}

ci::Matrix44f GameCamera::getProjectionMatrix() const
{
	return mCinderCamera->getProjectionMatrix();
}

const ci::Vec3f& GameCamera::getDirection() const
{
	return mLookDirection;
}

void GameCamera::setScreenSize( int width, int height, float contentScaleFactor )
{
	mScreenSize = ci::Vec2i( width, height );
	mScreenRect = ci::Rectf( 0, 0, width, height );
	mScreenAspectRatio = (float) width / (float) height;
	mContentScaleFactor = contentScaleFactor;
}

void GameCamera::setRange( float near, float far )
{
	mNear = near;
	mFar = far;
}

void GameCamera::setBodyOffset( const float x, const float y )
{
	mBody.position.x = x;
	mBody.position.y = y;
}

void GameCamera::setZoom( float zoom )
{
	mBody.position.z = zoom;
}

void GameCamera::setFov( float fov )
{
	mFov = fov;
}

ci::Vec2f GameCamera::getWorldToScreen( const ci::Vec3f &worldCoord ) const
{
	ci::Quatf q = ci::Quatf( Node::getTransform() );
	ci::Quatf f = ci::Quatf( ci::Vec3f::zAxis(), worldCoord - mBody.getTransform().getTranslate().xyz() );
	float dot = q.dot( f );
	if ( dot < 0.5f ) {
		return mCinderCamera->worldToScreen( worldCoord, mScreenSize.x, mScreenSize.y );
	}
	else {
		return ci::Vec2i(-99999,-99999);
	}
}

ci::Ray GameCamera::rayIntoScene( ci::Vec2i screenPoint )
{
	float u = (float) screenPoint.x / (float) mScreenSize.x;
	float v = (float) screenPoint.y / (float) mScreenSize.y;
	ci::Ray ray = mCinderCamera->generateRay(u , 1.0f - v, mCinderCamera->getAspectRatio() );
	return ray;
}

GameCamera::GameCamera()
{
	mCinderCamera = new ci::CameraPersp();
	
	mNear = 1.0f;
	mFar = 100.0f;
	mFov = 45.0f;
	
	mPivot.rotation.x	= -13.0f;
	mBody.position.z	= 30.0f;
	
	mBody.setParent( &mPivot );
	mPivot.setParent( this );
	
	update( 0.0f );
}

GameCamera::~GameCamera()
{
	delete mCinderCamera;
}

ci::Vec3f GameCamera::getGlobalPosition()
{
	return mBody.getGlobalPosition();
}

void GameCamera::update( const float deltaTime )
{	
	Node::update( deltaTime );
	mPivot.update( deltaTime );
	mBody.update( deltaTime );
	
	glViewport( 0, 0, mScreenSize.x, mScreenSize.x );
	mCinderCamera->setPerspective( mFov, mScreenAspectRatio, mNear, mFar );
	mCinderCamera->setWorldUp( ci::Vec3f::yAxis() );
	mCinderCamera->setEyePoint( mBody.getGlobalPosition() );
	mCinderCamera->setOrientation( ci::Quatf( mBody.getTransform() ) );
	
	mLookDirection = ( mCinderCamera->getEyePoint() - mCinderCamera->getCenterOfInterestPoint() ).normalized();
}