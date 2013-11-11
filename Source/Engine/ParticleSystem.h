#pragma once

#include "Node.h"
#include "cinder/Color.h"

#include <list>
#include <boost/function.hpp>

class Scene;
class ParticleSystem;

/*template<typename T>
class AnimValue {
public:
	AnimValue( T* valuePtr, boost::function<T(float,float)> randFunc ) : mValue( valuePtr ), mRandFunc( randFunc ) {}
	T startBase;
	T startRange;
	T velocityBase;
	T velocityRange;
	T velocity;
	T accelerationBase;
	T accelerationRange;
	T acceleration;
	
	void update( const float deltaTime )
	{
		*mValue += velocity;
		velocity *= acceleration;
	}
	
	void reset()
	{
		*mValue = startBase + mRandFunc(-1.0,1.0) * startRange;
		velocity = velocityBase + mRandFunc(-1.0,1.0) * velocityRange;
		acceleration = accelerationBase + mRandFunc(-1.0,1.0) * accelerationRange;
	}
private:
	boost::function<T(float,float)> mRandFunc;
	T* mValue;
};

typedef AnimValue<float> AnimValuef;
typedef AnimValue<ci::Vec3f> AnimValueVec3f;*/

class Particle {
public:
	
	Particle( ParticleSystem* system );
	~Particle();
	Node* mNode;
	
	void update( const float deltaTime );
	void reset();
	const float getElapsedLifetime() const { return elapsedTime / lifetime; }
	
	ci::Vec3f velocity;
	ci::Vec3f startPosition;
	ci::Vec3f scaleVelocity;
	float rotationalVelocity;
	float lifetime;
	
private:
	float elapsedTime;
	ParticleSystem* mSystem;
};

class ParticleSystem {
public:
	ParticleSystem( Scene* scene );
	virtual ~ParticleSystem();
	
	virtual void update( const float deltaTime );
	
	void emitOneShot();
	void emitLoop();
	void updateParticleCompletion();

	Node* mNode;
	
	void createParticles( int count, Texture* texture, bool clearExisting = false );
	void clearParticles();
	void setVelocity( const ci::Vec3f base, const ci::Vec3f range );
	void setRotationalVelocity( const float base, const float range );
	void setStartPositionRange( const ci::Vec3f range );
	void setScaleVelocity( const float base, const float range );
	
	const std::list<Particle*>& getParticles() const { return mParticles; }
	
	const bool getIsOneShot() const { return mOneShot; }
	const bool getIsExpired() const { return mIsExpired; }
	
	Scene* getScene() const { return mScene; }
	
private:
	Scene* mScene;
	bool mIsExpired;
	std::list<Particle*> mParticles;
	bool mOneShot;
};