#pragma once

#include "Node.h"
#include "LifeMeter.h"

#include "cinder/Vector.h"
#include "cinder/AxisAlignedBox.h"

class ObjectController {
public:
	ObjectController( Node* node, int faction = -1 );
	virtual ~ObjectController();
	
	void update( const float deltaTime );
	ci::AxisAlignedBox3f& getBoundingBox() { return mBoundingBox; }
	Node* getNode() const { return mNode; }
	
	int mFaction;
	
private:
	ci::Vec2i mScreenPosition;
	Node* mNode;
	//ActiveRecordRef mModel;
	ci::AxisAlignedBox3f mBoundingBox;
	LifeMeter mLifeMeter;
	ci::Vec3f mVelocity;
};
