#include "Node.h"
#include "RenderingEngine.h"
#include "GameConstants.h"

#include "cinder/CinderMath.h"

using namespace ci;

Node::Node() :
	mLayer( Node::LayerNone ),
	mIsDirty( true ),
	mMesh( NULL ),
	mParent( NULL ),
	mFaceCamera( false ),
	mFaceCameraAsLine( false )
{
	mTransform			= Matrix44f::identity();
	mLocalTransform		= Matrix44f::identity();
	position			= Vec3f::zero();
	rotation			= Vec3f::zero();
	scale				= Vec3f::one();
	pivotOffset			= Vec3f::zero();
	
	update();
}

Node::~Node()
{
	                              
}

void Node::setForward( const ci::Vec3f forward )
{
}

ci::Vec3f Node::getGlobalPosition()
{
	return Vec3f( mTransform.getTranslate().xyz() );
}

void Node::setParent( Node* parent )
{
	mParent = parent;
	mIsDirty = true;
}

void Node::update( const float deltaTime )
{
	mLocalTransform = Matrix44f::identity();
	mLocalTransform.translate( position );
	if ( !mFaceCamera ) {
		mLocalTransform.rotate( rotation * M_PI / 180.0f );
	}
	mLocalTransform.scale( scale );
	
	if ( mParent != NULL ) {
		mTransform = mParent->getTransform() * mLocalTransform;
	}
	else {
		mTransform = mLocalTransform;
	}
	
	if ( mFaceCamera ) {
		const Vec3f cameraDirection = ( Camera::get()->getGlobalPosition() - getGlobalPosition() ).normalized();
		mTransform *= ci::Matrix44f::alignZAxisWithTarget( cameraDirection, ci::Vec3f::yAxis() );
		mTransform.rotate( cross( cameraDirection, Vec3f::zAxis() ), mFaceCameraRotation * kToRad );
		
		updateCameraDistance();
	}
	
	if ( mFaceCameraAsLine ) {
		const Vec3f cameraDirection = ( Camera::get()->getGlobalPosition() - getGlobalPosition() ).normalized();
		mTransform *= ci::Matrix44f::alignZAxisWithTarget( cameraDirection, ci::Vec3f::yAxis() );
		
		updateCameraDistance();
	}
	
	for( auto delegate : mDelegates ) {
		delegate->update( deltaTime );
	}
}

void Node::updateCameraDistance()
{
	// Try to optimize this, don't do it every frame if possible:
	mDistanceFromCamera = Camera::get()->getGlobalPosition().distance( getGlobalPosition() );
}