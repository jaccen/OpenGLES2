#pragma once

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "cinder/Vector.h"
#include "cinder/Matrix.h"

#include "SmartValue.h"

class Texture;

struct VboMesh {
    GLuint					VertexBuffer;
	int						VertexCount;
    GLuint					TexCoordBuffer;
    int						TexCoordCount;
    GLuint					NormalBuffer;
    int						NormalCount;
    GLuint					IndexBuffer;
    int						IndexCount;
};

class Node {
public:
	Node();
	virtual~ Node();
	
	void					update( const float deltaTime );
	const ci::Matrix44f&	getTransform() const { return mTransform; }
	
	ci::Vec3f				position;
	ci::Vec3f				rotation;
	ci::Vec3f				scale;
	ci::Vec3f				pivotOffset;
	
	std::string				mShader;
	VboMesh					mMesh;
	ci::Vec4f				mColor;
	ci::Vec4f				mColorSpecular;
	ci::Vec4f				mColorSelfIllumination;
	float					mShininess;
	float					mGlossiness;
	Texture*				mTexture;
	Texture*				mTextureNormal;
	Texture*				mTextureSpecular;
	Texture*				mTextureAlpha;
	Texture*				mTextureSelfIllumination;
	
	Node*					mParent;
	bool					isDirty();
	
private:
	void					updateTransform();
	
	ci::Matrix44f			mTransform;
	ci::Vec3f				mLastPosition;
	ci::Vec3f				mLastRotation;
	ci::Vec3f				mLastScale;
};