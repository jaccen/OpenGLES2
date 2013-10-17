#include "ProjectileManager.h"

#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "Unit.h"
#include "UniformGrid.h"
#include "Game.h"

#include "cinder/Color.h"

#include <iostream>

using namespace ci;

ProjectileManager* ProjectileManager::sInstance = NULL;

ProjectileManager* ProjectileManager::get()
{
	if ( !sInstance ) {
		sInstance = new ProjectileManager();
	}
	return sInstance;
}

ProjectileManager::ProjectileManager() {}

ProjectileManager::~ProjectileManager() {}

void ProjectileManager::createProjectile( Unit* owner, const ci::Vec3f origin, const ci::Vec3f direction, const float speed )
{
	Projectile* proj = new Projectile( owner );
	proj->getNode()->position = origin + direction.normalized();
	proj->setDirection( direction.normalized() );
	proj->mSpeed = speed;
	mProjectiles.push_back( proj );
	
	mCollisionCheckTimer = ly::Timer( boost::bind( &ProjectileManager::checkCollisions, this, boost::arg<1>() ), 1.0f / 10.0f, 0 );
	mCollisionCheckTimer.start();
}

void ProjectileManager::createProjectileImpage( Node* parent, const ci::Vec3f localPosition )
{
	ParticleSystem* ps = new ParticleSystem();
	ps->createParticles( 1, ResourceManager::get()->getTexture( "textures/explosion2.png" ) );
	ps->setVelocity( Vec3f::zero(), Vec3f::zero() );
	ps->setRotationalVelocity( 0.0f, 50.0f );
	ps->setStartPositionRange( Vec3f::zero() );
	ps->setScaleVelocity( 10.0f, 10 );
	//ps->mNode->setParent( parent );
	ps->mNode->position = localPosition / 2.0f;
	ps->update( 0.0f );
	ps->emitOneShot();
	mImpacts.push_back( ps );
}

void ProjectileManager::update( const float deltaTime )
{
	for( auto p : mProjectiles ) {
		p->update( deltaTime );
		if ( p->getIsExpired() ) {
			mDeleteionQueue.push_back( p );
			continue;
		}
	}
	
	for( auto p : mDeleteionQueue ) {
		std::list<Projectile*>::iterator iter = std::find( mProjectiles.begin(), mProjectiles.end(), p );
		if ( iter != mProjectiles.end() ) {
			delete *iter;
			mProjectiles.erase( iter );
			//std::cout << "Projectile erased, " << mProjectiles.size() << " remaining." << std::endl;
		}
	}
	
	mDeleteionQueue.clear();
	
	for( auto ps : mImpacts ) {
		ps->update( deltaTime );
		if ( ps->getIsExpired() ) {
			mImpactDeletionQueue.push_back( ps );
		}
	}
	
	for( auto ps : mImpactDeletionQueue ) {
		std::list<ParticleSystem*>::iterator iter = std::find( mImpacts.begin(), mImpacts.end(), ps );
		if ( iter != mImpacts.end() ) {
			delete *iter;
			mImpacts.erase( iter );
		}
	}
	mImpactDeletionQueue.clear();
	
	mCollisionCheckTimer.update( deltaTime );
}

void ProjectileManager::checkCollisions( const float deltaTime )
{
	const std::vector<Unit*>& units = Game::get()->getUnits();
	
	for( auto p : mProjectiles ) {
		for( auto unit : units ) {
			Node* node = unit->getNode();
			if ( node->mLayer == Node::LayerObjects && unit != p->getOwner() ) {
				if ( node->getBoundingBox().getCenter().distance( p->getNode()->position ) < node->getBoundingBox().getSize().x * 0.5f ) {
					unit->onProjectileHit( p );
					if ( arc4random() % 10 == 5 ) {
						/*ci::Ray ray( p->getNode()->position, p->mForward );
						 float distance = 0.0f;
						 int len = node->mMesh->triMesh.getNumTriangles();
						 for( size_t i = 0; i < len; i++ ) {
						 Vec3f v0, v1, v2;
						 node->mMesh->triMesh.getTriangleVertices(i, &v0, &v1, &v2);
						 v0 = node->getTransform().transformPointAffine(v0);
						 v1 = node->getTransform().transformPointAffine(v1);
						 v2 = node->getTransform().transformPointAffine(v2);
						 if( ray.calcTriangleIntersection( v0, v1, v2, &distance ) ) {
						 const ci::Vec3f point = ray.getOrigin() + ray.getDirection() * distance;
						 createProjectileImpage( node, point );
						 }
						 }*/
						createProjectileImpage( node, p->getNode()->position );
					}
					mDeleteionQueue.push_back( p );
				}
			}
		}
	}
}