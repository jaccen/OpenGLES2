#include "Node.h"
#include "RenderingEngine.h"

#include "cinder/CinderMath.h"

using namespace ci;

const static float kToRad = 0.017453292519943295769f;
const static float kToDeg = 0.017453292519943295769;

void VboMesh::getTriangleVertices( size_t i, Vec3f *a, Vec3f *b, Vec3f *c ) const
{
	*a = indexOrderedVertices[ i+0  ];
	*b = indexOrderedVertices[ i+1 ];
	*c = indexOrderedVertices[ i+2 ];
}


Node::Node() :
	mIsDirty( true ),
	mMesh( NULL ),
	mTexture( NULL ),
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
	if ( isDirty() || ( mParent && mParent->isDirty() ) ) {
		updateTransform();
		mIsDirty = false;
	}
}

void Node::setForward( const ci::Vec3f forward )
{
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
}

