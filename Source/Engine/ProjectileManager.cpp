#include "ProjectileManager.h"
#include "Scene.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "Unit.h"
#include "UniformGrid.h"
#include "Game.h"
#include "RTSDemo.h"

#include "cinder/Color.h"

#include <iostream>

using namespace ci;

ProjectileManager::ProjectileManager() : mHitCounter(0)
{
	mCollisionCheckTimer = ly::Timer( boost::bind( &ProjectileManager::checkCollisions1, this, boost::arg<1>() ), 1.0f / 15.0f, 0 );
	mCollisionCheckTimer.start();
}

ProjectileManager::~ProjectileManager() {}

void ProjectileManager::createProjectile( Unit* owner, Unit* target, const float speed )
{
	if ( target == NULL || target->getIsDead() || target->getNode() == NULL ) return;
	
	Projectile* proj = new Projectile( owner, target, RTSDemo::get()->getMainScene() );
	const ci::Vec3f origin = owner->getNode()->position;
	const ci::Vec3f direction = target->getNode()->position - origin;
	proj->getNode()->position = origin + direction.normalized();
	proj->setDirection( direction.normalized() );
	proj->mSpeed = speed;
	mProjectiles.push_back( proj );
}

void ProjectileManager::createProjectileImpact( Node* parent, const ci::Vec3f localPosition )
{
	ParticleSystem* ps = new ParticleSystem( RTSDemo::get()->getMainScene() );
	ps->createParticles( 1, ResourceManager::get()->getTexture( "textures/explosion2.png" ) );
	ps->setVelocity( Vec3f::zero(), Vec3f::zero() );
	ps->setRotationalVelocity( 0.0f, 50.0f );
	ps->setStartPositionRange( Vec3f::one() * 0.1f );
	ps->setScaleVelocity( 10.0f, 10 );
	ps->mNode->setParent( parent );
	ps->mNode->position = localPosition;
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
	for( auto p : mProjectiles ) {
		if ( p->getIsExpired() ) continue;
		float testDistance = p->mSpeed * 0.3f;
		if ( Node* node = p->getTarget()->getNode() ) {
			ci::Ray ray( p->getNode()->getGlobalPosition(), p->mForward.normalized() );
			if ( node->getBoundingBox().intersects( ray ) ) {
				float distance = 0.0f;
				int len = node->getMesh()->triMesh.getNumTriangles();
				for( size_t i = 0; i < len; i++ ) {
					Vec3f v0, v1, v2;
					node->getMesh()->triMesh.getTriangleVertices( i, &v0, &v1, &v2 );
					v0 = node->getTransform().transformPointAffine( v0 );
					v1 = node->getTransform().transformPointAffine( v1 );
					v2 = node->getTransform().transformPointAffine( v2 );
					if( ray.calcTriangleIntersection( v0, v1, v2, &distance ) && distance < testDistance ) {
						if ( mHitCounter++ >= 2 ) {
							mHitCounter = 0;
							const ci::Vec3f point = ray.getOrigin() + ray.getDirection() * testDistance - node->getGlobalPosition();
							createProjectileImpact( node, point );
							//createProjectileImpact( node, p->getNode()->position );
						}
						p->getTarget()->onProjectileHit( p );
						mDeleteionQueue.push_back( p );
					}
				}
			}
		}
	}
}


void ProjectileManager::checkCollisions1( const float deltaTime )
{
	for( auto p : mProjectiles ) {
		if ( p->getIsExpired() ) continue;
		float testDistance = p->mSpeed * 0.3f;
		if ( Node* node = p->getTarget()->getNode() ) {
			const float bbRadius = node->getBoundingBox().getSize().x * 0.5f;
			if ( node->getBoundingBox().getCenter().distance( p->getNode()->position ) < bbRadius ) {
				if ( mHitCounter++ >= 2 ) {
					mHitCounter = 0;
					createProjectileImpact( node, randVec3fSphere( -bbRadius * 5.0f, bbRadius * 5.0f ) );
				}
				p->getTarget()->onProjectileHit( p );
				mDeleteionQueue.push_back( p );
			}
		}
	}
}

void ProjectileManager::checkCollisions2( const float deltaTime )
{
	const std::vector<Unit*>& units = RTSDemo::get()->getUnits();
	
	for( auto p : mProjectiles ) {
		for( auto unit : units ) {
			Node* node = unit->getNode();
			if ( node->mLayer == Node::LayerObjects && unit != p->getOwner() ) {
				if ( node->getBoundingBox().getCenter().distance( p->getNode()->position ) < node->getBoundingBox().getSize().x * 0.5f ) {
					/*ci::Ray ray( p->getNode()->position, p->mForward );
					float distance = 0.0f;
					int len = node->getMesh()->triMesh.getNumTriangles();
					for( size_t i = 0; i < len; i++ ) {
						Vec3f v0, v1, v2;
						node->getMesh()->triMesh.getTriangleVertices(i, &v0, &v1, &v2);
						v0 = node->getTransform().transformPointAffine(v0);
						v1 = node->getTransform().transformPointAffine(v1);
						v2 = node->getTransform().transformPointAffine(v2);
						if( ray.calcTriangleIntersection( v0, v1, v2, &distance ) && distance < node->getBoundingBox().getSize().x * 0.5f ) {
							const ci::Vec3f point = ray.getOrigin() + ray.getDirection() * distance / 2.0f;
							createProjectileImpact( node, point );
							mDeleteionQueue.push_back( p );
							//unit->onProjectileHit( p );
						}
					}*/
					unit->onProjectileHit( p );
					//createProjectileImpact( node, node->position + (p->getNode()->position - node->position).normalized() * node->getBoundingBox().getSize().x * 0.5f );
					createProjectileImpact( node, p->getNode()->position );
					mDeleteionQueue.push_back( p );
				}
			}
		}
	}
}