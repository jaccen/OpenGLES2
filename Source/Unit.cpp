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
	speed(100.0f),
	health(50.0f),
	maxHealth(10.0f),
	attackRange(750.0f),
	alertRange(1000.0f),
	factionId(-1),
	mAttackTarget( NULL ),
	mIsSelected( false ),
	mControlMode( HOLD ),
	mCurrentSpeed( 0.0f )
{
	mTimer = ly::Timer( boost::bind( &Unit::doTimedAttack, this, boost::arg<1>() ), attackSpeed, 0 );
	mTimer2 = ly::Timer( boost::bind( &Unit::doTimedInterval, this, boost::arg<1>() ), attackInterval, 0 );
	//mTimer2.start();
}

Unit::~Unit()
{
}

void Unit::update( const float deltaTime )
{
	if ( getIsDead() ) return;
	
	mTimer.update( deltaTime );
	mTimer2.update( deltaTime );
	
	updateDistanceCalculations(); // TODO: Put this on a timer
	mScreenPosition = GameCamera::get()->getWorldToScreen( getNode()->position );
	
	if ( mControlMode == HOLD ) {
		/*Unit* nearestVisibleEnemy = Game::get()->getUniformGrid().findNearest( Unit::FindVisibleEnemy( this ) );
		if ( nearestVisibleEnemy != NULL ) {
			// Move to the enemy
			getNode()->setForward( nearestVisibleEnemy->getNode()->position - getNode()->position );
			mAttackTarget = Game::get()->getUniformGrid().findNearest( Unit::FindAttackTarget( this ) );
		}*/
		
		mAttackTarget = Game::get()->getUniformGrid().findNearest( Unit::FindAttackTarget( this ) );
		if ( mAttackTarget != NULL && mAttackTarget->getIsDead() ) {
			getNode()->setForward( mAttackTarget->getNode()->position - getNode()->position );
		}
		mCurrentSpeed = 0.0;
	}
	else if ( mControlMode == MOVE ) {
		mAttackTarget= NULL;
		getNode()->setForward( mMoveTarget - getNode()->position );
		mTimer2.stop();
		if ( mDistanceFromMoveTarget <= 5.0f ) {
			mCurrentSpeed = 0.0;
		}
		else {
			mCurrentSpeed = speed;
		}
	}
	else if ( mControlMode == ATTACK ) {
		if ( mAttackTarget != NULL && !mAttackTarget->getIsDead() ) {
			getNode()->setForward( mAttackTarget->getNode()->position - getNode()->position );
			if ( mDistanceFromAttackTarget < attackRange ) {
				mTimer2.start( false, true );
				mCurrentSpeed = 0.0;
			}
			else {
				mTimer2.stop();
				mCurrentSpeed = speed;
			}
		}
		else {
			mControlMode = HOLD;
		}
	}
	
	getNode()->position += getNode()->getForward() * deltaTime * mCurrentSpeed;
}

void Unit::updateDistanceCalculations()
{
	if ( mAttackTarget != NULL ) {
		mDistanceFromAttackTarget = mAttackTarget->getNode()->position.distance( getNode()->position );
	}
	mDistanceFromMoveTarget = mMoveTarget.distance( getNode()->position );
}

void Unit::commandAttackTarget( Unit* target )
{
	mControlMode = ATTACK;
	mAttackTarget = target;
}

void Unit::commandMove( ci::Vec3f position )
{
	mControlMode = MOVE;
	mMoveTarget = position;
	mCurrentSpeed = speed;
}

void Unit::doTimedAttack( const float elapsedTime )
{
	if ( mAttackTarget == NULL || mAttackTarget->getIsDead() ) {
		mTimer2.stop();
		return;
	}
	
	const Vec3f origin = getNode()->getBoundingBox().getCenter();
	ProjectileManager::get()->createProjectile( this, mAttackTarget, 600.0f );
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