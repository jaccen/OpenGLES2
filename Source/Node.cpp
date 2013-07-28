#include "Node.h"
#include "RenderingEngine.h"
#include "GameConstants.h"

#include "cinder/CinderMath.h"

using namespace ci;

Node::Node() :
	mLayer( Node::LayerNone ),
	mIsDirty( true ),
	mMesh( NULL ),
	mTexture( NULL ),
	mTexture2( NULL ),
	mTexture3( NULL ),
	mTextureNormal( NULL ),
	mTextureSpecular( NULL ),
	mTextureAlpha( NULL ),
	mTextureSelfIllumination( NULL ),
	mParent( NULL ),
	mFaceCamera( false )
{
	mTransform			= Matrix44f::identity();
	mLocalTransform		= Matrix44f::identity();
	position			= Vec3f::zero();
	orientation			= Quatf::identity();
	scale				= Vec3f::one();
	mColor				= Vec4f::one();
	pivotOffset			= Vec3f::zero();
	mColorSpecular		= Vec4f::zero();
	mGlossiness			= 0.0f;
	mRimPower			= 0.0f;
	
	updateTransform();
}

Node::~Node()
{    
	                              
}


void Node::setForward( const ci::Vec3f forward )
{
	ci::Matrix44f trans = ci::Matrix44f::alignZAxisWithTarget( forward.normalized(), ci::Vec3f::yAxis() );
	orientation = Quatf( trans );
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

bool Node::isDirty()
{
	return  mIsDirty ||
			mLastPosition != position ||
			mLastOrientation != orientation ||
			mLastScale != scale;
}

void Node::update( const float deltaTime )
{
	// // Try to optimize mFaceCamera for spirtes, don't do it every frame if possible:
	if ( isDirty() || ( mParent && mParent->isDirty() ) || mFaceCamera ) {
		updateTransform();
		mIsDirty = false;
	}
}

void Node::updateTransform()
{
	mLocalTransform = Matrix44f::identity();
	mLocalTransform.translate( position );
	if ( !mFaceCamera ) {
		mLocalTransform *= orientation.toMatrix44();
	}
	//mLocalTransform.translate( pivotOffset );
	mLocalTransform.scale( scale );
	
	if ( mParent != NULL ) {
		mTransform = mParent->getTransform() * mLocalTransform;
	}
	else {
		mTransform = mLocalTransform;
	}
	
	if ( mFaceCamera ) {
		Vec3f cameraDirection = Camera::get()->getGlobalPosition() - getGlobalPosition();
		mTransform *= ci::Matrix44f::alignZAxisWithTarget( cameraDirection.normalized(), ci::Vec3f::yAxis() );
		// Try to optimize this, don't do it every frame if possible:
		//mDistanceFromCamera = Camera::get()->getGlobalPosition().distance( getGlobalPosition() );
	}
	
	mLastOrientation = orientation;
	mLastPosition = position;
	mLastScale = scale;
}

