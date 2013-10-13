#pragma once

#include "Node.h"
#include "cinder/Color.h"

#include <list>

class ParticleSystem;

class Particle {
public:
	Particle( ParticleSystem* system );
	Node mNode;
	
	void update( const float deltaTime );
	
	ci::Vec3f velocity;
	ci::Vec3f startPosition;
	float rotationalVelocity;
	float lifetime;
	
private:
	float elapsedTime;
	ParticleSystem* mSystem;
};

class ParticleSystem : public Node::IDelegate {
public:
	ParticleSystem();
	virtual ~ParticleSystem();
	
	virtual void update( const float deltaTime );

	Node mNode;
	
	void createParticles( int count, Texture* texture, bool clearExisting = false );
	void clearParticles();
	void setVelocity( const ci::Vec3f base, const ci::Vec3f range );
	void setRotationalVelocity( const float base, const float range );
	void setStartPositionRange( ci::Vec3f range );
	
	const std::list<Particle*>& getParticles() const { return mParticles; }
	
private:
	std::list<Particle*> mParticles;
};