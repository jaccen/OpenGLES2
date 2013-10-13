#include "Camera.h"
#include "GameConstants.h"

#include "cinder/Camera.h"
#include "cinder/Quaternion.h"

Camera* Camera::sInstance = NULL;

Camera* Camera::get()
{
	if ( !sInstance ) sInstance = new Camera();
	return sInstance;
}

ci::Matrix44f Camera::getModelViewMatrix() const
{
	return mCinderCamera->getModelViewMatrix();
}

ci::Matrix44f Camera::getProjectionMatrix() const
{
	return mCinderCamera->getProjectionMatrix();
}

void Camera::setScreenSize( int width, int height, float contentScaleFactor )
{
	mScreenSize = ci::Vec2i( width, height );
	mScreenRect = ci::Rectf( 0, 0, width, height );
	mScreenAspectRatio = (float) width / (float) height;
	mContentScaleFactor = contentScaleFactor;
}

void Camera::setRange( float near, float far )
{
	mNear = near;
	mFar = far;
}

void Camera::setZoom( float zoom )
{
	mBody.position.z = zoom;
}

void Camera::setFov( float fov )
{
	mFov = fov;
}

ci::Vec2f Camera::getWorldToScreen( const ci::Vec3f &worldCoord ) const
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

ci::Ray Camera::rayIntoScene( ci::Vec2i screenPoint )
{
	float u = (float) screenPoint.x / (float) mScreenSize.x;
	float v = (float) screenPoint.y / (float) mScreenSize.y;
	ci::Ray ray = mCinderCamera->generateRay(u , 1.0f - v, mCinderCamera->getAspectRatio() );
	return ray;
}

Camera::Camera()
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

Camera::~Camera()
{
	delete mCinderCamera;
}

ci::Vec3f Camera::getGlobalPosition()
{
	return mBody.getGlobalPosition();
}

void Camera::update( const float deltaTime )
{	
	Node::update( deltaTime );
	mPivot.update( deltaTime );
	mBody.update( deltaTime );
	
	glViewport( 0, 0, mScreenSize.x, mScreenSize.x );
	mCinderCamera->setPerspective( mFov, mScreenAspectRatio, mNear, mFar );
	mCinderCamera->setWorldUp( ci::Vec3f::yAxis() );
	mCinderCamera->setEyePoint( mBody.getGlobalPosition() );
	mCinderCamera->setOrientation( ci::Quatf( mBody.getTransform() ) );
	
}