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
	rotation			= Vec3f::zero();
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
	ci::Matrix44f trans = mTransform * ci::Matrix44f::alignZAxisWithTarget( forward.normalized(), ci::Vec3f::yAxis() );
	Quatf q( trans );
	float rads;
	q.getAxisAngle( &rotation, &rads );
	rotation *= kToDeg;
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
			mLastRotation != rotation ||
			mLastScale != scale;
}

void Node::update( const float deltaTime )
{
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
		mLocalTransform.rotate( rotation * kToRad );
	}
	mLocalTransform.translate( pivotOffset );
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
		//mTransform.rotate( Vec3f::zAxis(), toRadians( mSpriteRotation ) );
	}
	
	mLastRotation = rotation;
	mLastPosition = position;
	mLastScale = scale;
}

