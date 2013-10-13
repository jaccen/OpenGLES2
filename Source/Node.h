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
	const ci::Matrix44f&	getLocalTransform() const { return mLocalTransform; }
	const ci::Matrix44f&	getTransform() const { return mTransform; }
	
	ci::Vec3f				position;
	ci::Vec3f				rotation;
	ci::Vec3f				scale;
	ci::Vec3f				pivotOffset;
	bool					mFaceCamera;
	bool					mFaceCameraAsLine;
	float					mFaceCameraRotation;
	
	Mesh*					mMesh;
	
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
	
private:
	inline void				updateCameraDistance();
	
	Node*					mParent;
	bool					mIsDirty;
	ci::Matrix44f			mTransform;
	ci::Matrix44f			mLocalTransform;
	ci::Vec3f				mLastScale;
	float					mDistanceFromCamera;
	
	std::list<IDelegate*>	mDelegates;
};