#pragma once

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "cinder/Vector.h"
#include "cinder/Matrix.h"

#include "SmartValue.h"


class Texture {
public:
	~Texture() { free( mImageData ); }
	GLuint					mHandle;
	int						mWidth;
	int						mHeight;
	void*					mImageData;
};

class VboMesh {
public:
	VboMesh() : vertexBuffer(-1), normalBuffer(-1), texCoordBuffer(-1), indexBuffer(-1) {}
    GLuint					vertexBuffer;
	int						vertexCount;
    GLuint					texCoordBuffer;
    int						texCoordCount;
    GLuint					normalBuffer;
    int						normalCount;
    GLuint					indexBuffer;
    int						indexCount;
};

class Node {
public:
	Node();
	virtual~ Node();
	
	void					update( const float deltaTime = 0.0f );
	const ci::Matrix44f&	getLocalTransform() const { return mLocalTransform; }
	const ci::Matrix44f&	getTransform() const { return mTransform; }
	
	ci::Vec3f				position;
	ci::Vec3f				rotation;
	ci::Vec3f				scale;
	ci::Vec3f				pivotOffset;
	bool					mFaceCamera;
	
	std::string				mShader;
	VboMesh*				mMesh;
	ci::Vec4f				mColor;
	ci::Vec4f				mColorSpecular;
	ci::Vec4f				mColorRim;
	ci::Vec4f				mColorSelfIllumination;
	float					mRimPower;
	float					mShininess;
	float					mGlossiness;
	Texture*				mTexture;
	Texture*				mTextureNormal;
	Texture*				mTextureSpecular;
	Texture*				mTextureAlpha;
	Texture*				mTextureSelfIllumination;
	
	void					setParent( Node* parent );
	Node*					getParent() const { return mParent; }
	bool					isDirty();
	
	ci::Vec3f				getGlobalPosition();
	void					setForward( const ci::Vec3f forward );
	
private:
	void					updateTransform();
	
	Node*					mParent;
	bool					mIsDirty;
	ci::Matrix44f			mTransform;
	ci::Matrix44f			mLocalTransform;
	ci::Vec3f				mLastPosition;
	ci::Vec3f				mLastRotation;
	ci::Vec3f				mLastScale;
};