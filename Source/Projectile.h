#pragma once

#include "Node.h"

class Projectile : public Node::IDelegate {
public:
	Projectile();
	virtual ~Projectile();
	Node mNode;
	ci::Vec3f mForward;
	float mSpeed;
	
	virtual void update( const float deltaTime );
	
private:
	
};