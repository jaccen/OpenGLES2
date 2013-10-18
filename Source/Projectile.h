#pragma once

#include "Node.h"

class Unit;

class Projectile : public Node::IDelegate {
public:
	Projectile( Unit* owner, Unit* target );
	virtual ~Projectile();
	ci::Vec3f mForward;
	float mSpeed;
	
	virtual void update( const float deltaTime );
	void setDirection( const ci::Vec3f direction );
	
	Node* getNode() const { return mBody; }
	Unit* getOwner() const { return mOwner; }
	Unit* getTarget() const { return mTarget; }
	
	bool getIsExpired() const { return mElapsedTime >= mLifetime; }
	
private:
	float mElapsedTime;
	float mLifetime;
	Node* mContainer;
	Node* mBody;
	Unit* mOwner;
	Unit* mTarget;
};