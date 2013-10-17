#pragma once

#include "Node.h"
#include "Timer.h"
#include "Projectile.h"

class Unit {
public:
	typedef enum {
		UNDEFINED = -1,
		HOLD,
		MOVE,
		ATTACK
	} ControlMode;
	
	static bool sortByDistanceFromCamera( Unit* a, Unit* b ) { return a->getNode()->getDistanceFromCamera() < b->getNode()->getDistanceFromCamera(); }

	class Predicate {
	public:
		Predicate() : mUnit( NULL ) {}
		Predicate( Unit* unit ) : mUnit( unit ) {}
		virtual bool operator()( Unit* other ) const = 0;
		const Unit* getUnit() const { return mUnit; }
		virtual const float getRange() const { return 0; }
	protected:
		Unit* mUnit;
	};
	
	class FindVisibleEnemy : public Predicate {
	public:
		FindVisibleEnemy() : Predicate( NULL ) {}
		FindVisibleEnemy( Unit* unit ) : Predicate( unit ) {}
		virtual bool operator()( Unit* other ) const {
			return	other != mUnit &&
			other->factionId != mUnit->factionId &&
			other->getNode()->position.distance( mUnit->getNode()->position ) < mUnit->alertRange;
		}
		virtual const float getRange() const { return mUnit->alertRange; }
	};
	
	class FindEnemy : public Predicate {
	public:
		FindEnemy() : Predicate( NULL ) {}
		FindEnemy( Unit* unit ) : Predicate( unit ) {}
		virtual bool operator()( Unit* other ) const {
			return	other != mUnit &&
			other->factionId != mUnit->factionId;
		}
		virtual const float getRange() const { return mUnit->alertRange; }
	};
	
	class FindAttackTarget : public Predicate {
	public:
		FindAttackTarget() : Predicate( NULL ) {}
		FindAttackTarget( Unit* unit ) : Predicate( unit ) {}
		virtual bool operator()( Unit* other ) const {
			return	other != mUnit &&
					other->factionId != mUnit->factionId &&
					other->getNode()->position.distance( mUnit->getNode()->position ) < mUnit->attackRange;
		}
		virtual const float getRange() const { return mUnit->attackRange; }
	};
	
	Unit( Node* node );
	virtual ~Unit();
	
	void doTimedAttack( const float elapsedTime );
	void doTimedInterval( const float elapsedTime );
	void update( const float deltaTime );
	
	void updateDistanceCalculations();
	
	void commandAttackTarget( Unit* target );
	void commandMove( ci::Vec3f position );
	
	void onProjectileHit( Projectile* projectile );
	const bool getIsDead() const { return health <= 0.0f; }
	
	Node* getNode() const { return mNode; }
	const ci::Vec2i& getScreenPosition() const { return mScreenPosition; }
	const ControlMode getControlMode() const { return mControlMode; }
	Unit* getAttackTarget() const { return mAttackTarget; }
	const ci::Vec3f& getMoveTarget() const { return mMoveTarget; }
	
	float	attackSpeed;
	float	attackDamage;
	float	attackInterval;
	float	speed;
	float	health;
	float	maxHealth;
	int		attackRange;
	int		alertRange;
	int		factionId;
	
	bool	mIsSelected;
	
private:
	float mDistanceFromAttackTarget;
	float mDistanceFromMoveTarget;
	
	float mCurrentSpeed;
	ControlMode mControlMode;
	Node* mNode;
	ci::Vec3f mMoveTarget;
	ci::Vec2i mScreenPosition;
	
	Unit* mAttackTarget;
	
	ly::Timer mTimer;
	ly::Timer mTimer2;
};