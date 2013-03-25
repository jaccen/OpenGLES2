#include "ObjectController.h"

using namespace ci;

ObjectController::ObjectController( Node* node, int faction ) : mNode( node ), mFaction(faction)
{
	mLifeMeter.setOwner( this );
	
	mNode->update();
	
	// Create a large bounding box
	Matrix44f t( node->getTransform() );
	t.scale( Vec3f::one() * 20.0f );
	mBoundingBox = node->mMesh->triMesh.calcBoundingBox( t );
}

ObjectController::~ObjectController() {}

void ObjectController::update( const float deltaTime )
{
	mLifeMeter.update( deltaTime );
}