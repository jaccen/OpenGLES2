#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/TriMesh.h"

#include "SmartValue.h"
#include "Resources.h"
#include "Material.h"

#include <vector>
#include <list>

class Node {
public:
	typedef enum {
		LayerNone		= 1 << 0,
		LayerLighting	= 1 << 1,
		LayerObjects	= 1 << 2,
		LayerGui		= 1 << 3,
		LayerClouds		= 1 << 4
	} Layer;
	
	static bool				sortByDistanceFromCamera( Node* a, Node* b ) { return a->getDistanceFromCamera() > b->getDistanceFromCamera(); }		
	
	class IDelegate {
	public:
		virtual void update( const float deltaTime ) = 0;
	};
	
	Node();
	virtual~ Node();
	
	void					update( const float deltaTime = 0.0f );
	const ci::Matrix44f&	getTransform() const { return mTransform; }
	
	ci::Vec3f				position;
	ci::Vec3f				rotation;
	ci::Vec3f				scale;
	ci::Vec3f				pivotOffset;
	bool					mFaceCamera;
	bool					mFaceCameraAsLine;
	float					mFaceCameraRotation;
	
	const ci::Vec3f			getForward() const { return mTransform.transformVec( ci::Vec3f::zAxis() ).normalized(); }
	const ci::Vec3f			getBack() const { return mTransform.transformVec( -ci::Vec3f::zAxis() ).normalized(); }
	const ci::Vec3f			getLeft() const { return mTransform.transformVec( -ci::Vec3f::xAxis() ).normalized(); }
	const ci::Vec3f			getRight() const { return mTransform.transformVec( ci::Vec3f::xAxis() ).normalized(); }
	const ci::Vec3f			getDown() const { return mTransform.transformVec( -ci::Vec3f::yAxis() ).normalized(); }
	const ci::Vec3f			getUp() const { return mTransform.transformVec( ci::Vec3f::yAxis() ).normalized(); }
	
	Material				mMaterial;
	const Material&			getMaterial() const { return mMaterial; }
	
	void					setParent( Node* parent );
	Node*					getParent() const { return mParent; }
	const float&			getDistanceFromCamera() { return mDistanceFromCamera; }
	ci::Vec3f				getGlobalPosition();
	void					setForward( const ci::Vec3f forward );
	
	Layer					mLayer;
	
	void					addDelegate( IDelegate* delegate ) { mDelegates.push_back( delegate ); }
	void					removeDelegate( IDelegate* delegate ) {} // TODO: MAke this work
	
	const ci::AxisAlignedBox3f& getBoundingBox() const { return mBoundingBox; }
	ci::AxisAlignedBox3f&	getBoundingBox() { return mBoundingBox; }
	
	ci::Matrix44f			mSpriteTransform;
	
	void					setMesh( Mesh* mesh );
	Mesh*					getMesh() const { return mMesh; }
	
private:
	
	Mesh*					mMesh;
	inline void				updateCameraDistance();
	
	Node*					mParent;
	ci::Matrix44f			mTransform;
	ci::Matrix44f			mLocalTransform;
	ci::Vec3f				mLastScale;
	float					mDistanceFromCamera;
	
	std::list<IDelegate*>	mDelegates;
	
	ci::AxisAlignedBox3f	mBoundingBox;
	ci::AxisAlignedBox3f	mObjectBounds;
};