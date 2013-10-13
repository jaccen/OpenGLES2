#include "ParticleSystem.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "GameConstants.h"

#include <iostream>

using namespace ci;

Particle::Particle( ParticleSystem* system ) : mSystem( system ), elapsedTime( 0.0f )
{
	velocity = Vec3f::zero();
	startPosition = Vec3f::zero();
	rotationalVelocity = 0.0f;
	lifetime = 2.50f;
	elapsedTime = randFloat() * lifetime;
}

void Particle::update( const float deltaTime )
{
	elapsedTime += deltaTime;
	
	const float r = elapsedTime / lifetime;
	mNode.mMaterial.mColors[ "DiffuseMaterial" ].a = r >= 0.5f ? (1.0f - r) * 2.0f : r * 6.0f;
	
	if ( elapsedTime >= lifetime ) {
		elapsedTime = 0.0f;
		mNode.position = mSystem->mNode.position + startPosition;
	}
	
	mNode.position += velocity * deltaTime;
	mNode.mFaceCameraRotation += rotationalVelocity * deltaTime;
}

ParticleSystem::ParticleSystem()
{
	mNode.addDelegate( this );
	RenderingEngine::get()->addNode( &mNode );
}

ParticleSystem::~ParticleSystem()
{
	clearParticles();
}

void ParticleSystem::createParticles( int count, Texture *texture, bool clearExisting )
{
	if ( clearExisting ) {
		clearParticles();
	}
	for( int i = 0; i < count; i++ ) {
		Particle* p = new Particle( this );
		p->mNode.mMaterial.setTexture( "DiffuseTexture", texture );
		p->mNode.mLayer = Node::LayerLighting;
		p->mNode.mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
		p->mNode.mMaterial.mShader = ResourceManager::get()->getShader( "unlit" );
		p->mNode.mMesh = ResourceManager::get()->getMesh( "models/quad_plane.obj" );
		p->mNode.mMaterial.setProperty( "Scale",	Vec2f::one() );
		p->mNode.scale = Vec3f::one() * 100.0f;
		p->mNode.mFaceCamera = true;
		RenderingEngine::get()->addSpriteNode( &p->mNode );
		mParticles.push_back( p );
	}
}

void ParticleSystem::clearParticles()
{
	for( auto p : mParticles ) {
		RenderingEngine::get()->removeNode( &p->mNode );
		delete p;
	}
	RenderingEngine::get()->removeNode( &mNode );
	mParticles.clear();
}

void ParticleSystem::update( const float deltaTime )
{
	for( auto p : mParticles ) {
		p->update( deltaTime );
	}
}

void ParticleSystem::setVelocity( const ci::Vec3f base, const ci::Vec3f range )
{
	for( auto p : mParticles ) {
		p->velocity = base + Vec3f( range.x * randFloat(), range.y * randFloat(), range.z * randFloat() );
	}
}

void ParticleSystem::setRotationalVelocity( const float base, const float range )
{
	for( auto p : mParticles ) {
		p->rotationalVelocity = base + randFloat( -1.0f, 1.0f ) * range;
	}
}

void ParticleSystem::setStartPositionRange( ci::Vec3f range )
{
	for( auto p : mParticles ) {
		p->startPosition = Vec3f( range.x * randFloat( -1.0f, 1.0f ), range.y * randFloat( -1.0f, 1.0f ), range.z * randFloat( -1.0f, 1.0f ) );
	}
}