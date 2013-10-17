#include "Node.h"
#include "RenderingEngine.h"
#include "GameConstants.h"

#include "cinder/CinderMath.h"

using namespace ci;

Node::Node() :
	mLayer( Node::LayerNone ),
	mMesh( NULL ),
	mParent( NULL ),
	mFaceCamera( false ),
	mFaceCameraAsLine( false ),
	mFaceCameraRotation( 0.0f )
{
	mSpriteTransform	= Matrix44f::identity();
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

void Node::setMesh( Mesh *mesh )
{
	mMesh = mesh;
	mObjectBounds = mMesh->triMesh.calcBoundingBox( mTransform );
}

void Node::setForward( const ci::Vec3f forward )
{
	if ( forward == Vec3f::zero() ) return;
	
	Quatf q( Matrix44f::alignZAxisWithTarget( forward.normalized(), Vec3f::yAxis() ) );
	Vec3f axis;
	float angle;
	q.getAxisAngle( &axis, &angle );
	rotation = axis * angle * kToDeg;
}

ci::Vec3f Node::getGlobalPosition()
{
	return Vec3f( mTransform.getTranslate().xyz() );
}

void Node::setParent( Node* parent )
{
	mParent = parent;
}

void Node::update( const float deltaTime )
{
	mLocalTransform = Matrix44f::identity();
	mLocalTransform.translate( position );
	if ( !mFaceCamera ) {
		mLocalTransform.rotate( rotation * kToRad );
	}
	mLocalTransform.scale( scale );
	
	if ( mParent != NULL ) {
		mTransform = mParent->getTransform() * mLocalTransform;
	}
	else {
		mTransform = mLocalTransform;
		if ( !mFaceCamera && !mFaceCameraAsLine ) {
		}
	}
	if ( mMesh != NULL ) {
		mBoundingBox = mObjectBounds.transformed( mTransform );
	}
	
	if ( mFaceCamera ) {
		mSpriteTransform = ci::Matrix44f::alignZAxisWithTarget( -GameCamera::get()->getDirection(), ci::Vec3f::yAxis() );
		mSpriteTransform.rotate( Vec3f::zAxis(), mFaceCameraRotation * kToRad );
		updateCameraDistance();
	}
	else if ( mFaceCameraAsLine ) {
		const Vec3f cam = GameCamera::get()->getGlobalPosition();
		const Vec3f axis = Vec3f::xAxis();
		const Vec3f adjustedPos = Vec3f( getGlobalPosition().x, cam.y, cam.z );
		const Vec3f dir = ( getGlobalPosition() - adjustedPos ).normalized();
		mSpriteTransform = ci::Matrix44f::alignZAxisWithTarget( dir, ci::Vec3f::yAxis() );
		updateCameraDistance();
	}
	
	for( auto delegate : mDelegates ) {
		delegate->update( deltaTime );
	}
}

void Node::updateCameraDistance()
{
	// Try to optimize this, don't do it every frame if possible:
	mDistanceFromCamera = GameCamera::get()->getGlobalPosition().distance( getGlobalPosition() );
}