#include "Unit.h"
#include "ProjectileManager.h"
#include "Game.h"
#include "UniformGrid.h"

#include <iostream>
#include <boost/bind.hpp>

using namespace ci;

Unit::Unit( Node* node ) :
	mNode( node ),
	attackSpeed(.1f),
	attackInterval(2.0f),
	attackDamage(2.0f),
	speed(50.0f),
	health(10.0f),
	maxHealth(10.0f),
	attackRange(200.0f),
	alertRange(1000.0f),
	factionId(-1),
	mAttackTarget( NULL )
{
	mTimer = ly::Timer( boost::bind( &Unit::doTimedAttack, this, boost::arg<1>() ), attackSpeed, 0 );
	
	mTimer2 = ly::Timer( boost::bind( &Unit::doTimedInterval, this, boost::arg<1>() ), attackInterval, 0 );
	//mTimer2.start();
	
	getNode()->setForward( Vec3f::zero() - getNode()->position );
}

Unit::~Unit()
{
}

void Unit::update( const float deltaTime )
{
	if ( getIsDead() ) return;
	
	mTimer.update( deltaTime );
	mTimer2.update( deltaTime );
	
	Unit* nearestVisibleEnemy = Game::get()->getUniformGrid().findNearest( Unit::FindVisibleEnemy( this ) );
	if ( nearestVisibleEnemy != NULL ) {
		// Move to the enemy
		getNode()->setForward( nearestVisibleEnemy->getNode()->position - getNode()->position );
		
		mAttackTarget = Game::get()->getUniformGrid().findNearest( Unit::FindAttackTarget( this ) );
		if ( mAttackTarget != NULL ) {
			if ( mAttackTarget->getIsDead() ) {
				mAttackTarget = NULL;
				return;
			}
			else {
				mTimer2.start( false, true );
			}
		}
		
		if ( mAttackTarget == NULL ) {
			getNode()->position += getNode()->getForward() * deltaTime * speed;
			mTimer2.stop();
		}
	}
}

void Unit::doTimedAttack( const float elapsedTime )
{
	if ( mAttackTarget == NULL || mAttackTarget->getIsDead() ) {
		mTimer2.stop();
		return;
	}
	
	const Vec3f origin = getNode()->getBoundingBox().getCenter();
	ProjectileManager::get()->createProjectile( this, origin, mAttackTarget->getNode()->getBoundingBox().getCenter() - origin, 600.0f );
}

void Unit::doTimedInterval( const float elapsedTime )
{
	mTimer.toggle();
}

void Unit::onProjectileHit( Projectile* projectile )
{
	health -= projectile->getOwner()->attackDamage;
	if ( health <= 0.0f ) {
		health = 0.0f;
		mTimer.stop();
		mTimer2.stop();
	}
}