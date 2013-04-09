#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/TriMesh.h"

#include "SmartValue.h"
#include "Resources.h"

#include <vector>

class Node {
public:
	typedef enum {
		LayerNone		= 1 << 0,
		LayerLighting	= 1 << 1,
		LayerObjects	= 1 << 2,
		LayerGui		= 1 << 3,
		LayerClouds		= 1 << 4
	} Layer;
	
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
	Mesh*					mMesh;
	ci::Vec4f				mColor;
	ci::Vec4f				mColorSpecular;
	ci::Vec4f				mColorRim;
	ci::Vec4f				mColorSelfIllumination;
	float					mRimPower;
	float					mShininess;
	float					mGlossiness;
	Texture*				mTexture;
	Texture*				mTexture2;
	Texture*				mTexture3;
	Texture*				mTextureNormal;
	Texture*				mTextureSpecular;
	Texture*				mTextureAlpha;
	Texture*				mTextureSelfIllumination;
	
	void					setParent( Node* parent );
	Node*					getParent() const { return mParent; }
	bool					isDirty();
	
	ci::Vec3f				getGlobalPosition();
	void					setForward( const ci::Vec3f forward );
	
	Layer					mLayer;
	
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