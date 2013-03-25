#include "Node.h"
#include "RenderingEngine.h"
#include "GameConstants.h"

#include "cinder/CinderMath.h"

using namespace ci;


Texture::Texture( int width, int height ) : mFormat( GL_RGBA )
{
	setSize( width, height );
}

ci::Vec2i Texture::getScaledTextureSize() const
{
	return ci::Vec2i( mWidth, mHeight ) / RenderingEngine::get()->getContentScaleFactor();
}

void Texture::setSize( int width, int height )
{
	int sizes[9] = { 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };
	for ( int i = 1; i < 9; i++ ) {
		if ( width <= sizes[i] && width > sizes[i-1] ) {
			mWidth = sizes[i];
		}
		if ( height <= sizes[i] && height > sizes[i-1] ) {
			mHeight = sizes[i];
		}
	}
}

FramebufferObject::FramebufferObject( int width, int height ) : mTexture(NULL), mWidth( width ), mHeight( height ), mFormat( GL_RGBA4 )
{
	
}

FramebufferObject::FramebufferObject( Texture* texture ) : mTexture( texture ), mWidth( texture->mWidth ), mHeight( texture->mHeight ), mFormat( texture->mFormat )
{
	
}

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
	if ( isDirty() || ( mParent && mParent->isDirty() ) ) {
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
}

