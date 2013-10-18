#include "Planet.h"

Planet::Planet( Node* node ) : mNode( node )
{
	mNode->addDelegate( this );
}

Planet::~Planet() {}

void Planet::update( const float deltaTime )
{
	//mNode->scale *= ci::Vec3f::one() * 1.01f;
	mNode->rotation.y += 30.0f * deltaTime;
	//mNode->position.x += 30.0f * deltaTime;
}