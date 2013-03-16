#include "Node.h"
#include "RenderingEngine.h"

using namespace ci;

const static float kToRad = 0.017453292519943295769f;
const static float kToDeg = 0.017453292519943295769;

Node::Node() :
	mTexture( NULL ),
	mTextureNormal( NULL ),
	mTextureSpecular( NULL ),
	mTextureAlpha( NULL ),
	mTextureSelfIllumination( NULL ),
	mParent( NULL )
{
	mTransform			= Matrix44f::identity();
	position			= Vec3f::zero();
	rotation			= Vec3f::zero();
	scale				= Vec3f::one();
	mColor				= Vec4f::one();
	pivotOffset			= Vec3f::zero();
	mColorSpecular		= Vec4f::zero();
	mGlossiness			= 0.0f;
	
	updateTransform();
}

Node::~Node()
{    
	                              
}

bool Node::isDirty()
{
	return	mLastPosition != position ||
			mLastRotation != rotation ||
			mLastScale != scale;
}

void Node::update( const float deltaTime )
{
	if ( isDirty() || ( mParent && mParent->isDirty() ) ) {
		updateTransform();
	}
}

void Node::updateTransform()
{
	mTransform = Matrix44f::identity();
	if ( mParent != NULL ) {
		mTransform = mParent->getTransform();
	}
	mTransform.translate( position );
	mTransform.rotate( rotation * kToRad );
	mTransform.translate( pivotOffset );
	mTransform.scale( scale );
}

