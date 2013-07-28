#include "ObjectController.h"

using namespace ci;

ObjectController::ObjectController( Node* node, int faction ) : mNode( node ), mFaction(faction), mShouldMove( false )
{
	//mLifeMeter.setOwner( this );
	mMoveTarget = Vec3f::zero();
	mNode->update();
	
	// Create a large bounding box
	Matrix44f t( node->getTransform() );
	t.scale( Vec3f::one() * 1.1f );
	mBoundingBox = node->mMesh->triMesh.calcBoundingBox( t );
}

ObjectController::~ObjectController() {}

void ObjectController::setMoveTarget( ci::Vec3f target )
{
	mShouldMove = true;
	mMoveTarget = target;
}

void ObjectController::update( const float deltaTime )
{	
	//mLifeMeter.update( deltaTime );
	
	if  ( mShouldMove ) {
		Vec3f direction = (mMoveTarget - mNode->position).normalized();
		float speed = 3.0f;
		mNode->setForward( direction );
		mNode->position += direction * deltaTime * speed;
	}
}