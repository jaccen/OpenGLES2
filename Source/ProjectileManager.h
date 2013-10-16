#pragma once

#include "Projectile.h"
#include "ParticleSystem.h"
#include "Timer.h"

class ProjectileManager {
public:
	static ProjectileManager* get();
	
	void createProjectile( Unit* owner, const ci::Vec3f origin, const ci::Vec3f direction, const float speed );
	void update( const float deltaTime );
	void checkCollisions( const float deltaTime );
	void createProjectileImpage( Node* parent, const ci::Vec3f localPosition );
	
private:
	ly::Timer mCollisionCheckTimer;
	
	static ProjectileManager* sInstance;
	ProjectileManager();
	virtual ~ProjectileManager();
	
	std::list<Projectile*> mProjectiles;
	std::list<Projectile*> mDeleteionQueue;
	std::list<ParticleSystem*> mImpacts;
	std::list<ParticleSystem*> mImpactDeletionQueue;
};