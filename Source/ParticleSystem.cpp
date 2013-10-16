#include "ParticleSystem.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "GameConstants.h"

#include <iostream>

using namespace ci;

Particle::Particle( ParticleSystem* system ) : mSystem( system ), elapsedTime( 0.0f )
{
	mNode = new Node();
	mNode->setParent( mSystem->mNode );
	RenderingEngine::get()->addSpriteNode( mNode );
	velocity = Vec3f::zero();
	startPosition = Vec3f::zero();
	rotationalVelocity = 0.0f;
	lifetime = 0.75f;
}

void Particle::reset()
{
	elapsedTime = 0.0f;
	mNode->position = mSystem->mNode->position + startPosition;
	mNode->scale = Vec3f::one() * 30.0f;
}

void Particle::update( const float deltaTime )
{
	if ( getElapsedLifetime() >= 1.0f ) return;
	
	elapsedTime += deltaTime;
	const float r = getElapsedLifetime();
	
	mNode->mMaterial.mColors[ "DiffuseMaterial" ].a = r >= 0.5f ? (1.0f - r) * 2.0f : r * 6.0f;
	mNode->scale				+= scaleVelocity * deltaTime;
	mNode->position				+= velocity * deltaTime;
	mNode->mFaceCameraRotation	+= rotationalVelocity * deltaTime;
	
	if ( r >= 1.0f ) {
		if ( !mSystem->getIsOneShot() ) {
			reset();
		}
		else {
			mSystem->updateParticleCompletion();
		}
	}
}

Particle::~Particle()
{
	RenderingEngine::get()->removeNode( mNode );
}

ParticleSystem::ParticleSystem() : mOneShot( false ), mIsExpired( false )
{
	mNode = new Node();
	RenderingEngine::get()->addSpriteNode( mNode );
}

ParticleSystem::~ParticleSystem()
{
	clearParticles();
	RenderingEngine::get()->removeNode( mNode );
}

void ParticleSystem::emitOneShot()
{
	mOneShot = true;
	for( auto p : mParticles ) {
		p->reset();
	}
	updateParticleCompletion();
}

void ParticleSystem::emitLoop()
{
	mOneShot = false;
	for( auto p : mParticles ) {
		p->reset();
	}
	updateParticleCompletion();
}

void ParticleSystem::updateParticleCompletion()
{
	for( auto p : mParticles ) {
		if ( p->getElapsedLifetime() < 1.0f ) {
			mIsExpired = false;
			return;
		}
	}
	mIsExpired = true;
}

void ParticleSystem::createParticles( int count, Texture *texture, bool clearExisting )
{
	if ( clearExisting ) {
		clearParticles();
	}
	for( int i = 0; i < count; i++ ) {
		Particle* p = new Particle( this );
		p->mNode->mLayer = Node::LayerLighting;
		p->mNode->mMaterial.setTexture( "DiffuseTexture", texture );
		p->mNode->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
		p->mNode->mMaterial.mShader = ResourceManager::get()->getShader( "unlit" );
		p->mNode->setMesh( ResourceManager::get()->getMesh( "models/quad_plane.obj" ) );
		p->mNode->mMaterial.setProperty( "Scale", Vec2f::one() );
		p->mNode->scale = Vec3f::one() * 0.1f;
		p->mNode->mFaceCamera = true;
		mParticles.push_back( p );
	}
}

void ParticleSystem::clearParticles()
{
	for( auto p : mParticles ) {
		delete p;
	}
	RenderingEngine::get()->removeNode( mNode );
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
		p->velocity = base + Vec3f( range.x * randFloat(-1,1), range.y * randFloat(-1,1), range.z * randFloat(-1,1) );
	}
}

void ParticleSystem::setRotationalVelocity( const float base, const float range )
{
	for( auto p : mParticles ) {
		p->rotationalVelocity = base + randFloat( -1.0f, 1.0f ) * range;
	}
}

void ParticleSystem::setStartPositionRange( const ci::Vec3f range )
{
	for( auto p : mParticles ) {
		p->startPosition = Vec3f( range.x * randFloat( -1.0f, 1.0f ), range.y * randFloat( -1.0f, 1.0f ), range.z * randFloat( -1.0f, 1.0f ) );
	}
}

void ParticleSystem::setScaleVelocity( const float base, const float range )
{
	for( auto p : mParticles ) {
		const float s = base + range * randFloat( -1.0f, 1.0f );
		p->scaleVelocity = Vec3f( s, s, s );
	}
}