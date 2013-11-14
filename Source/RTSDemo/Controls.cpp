#include "Controls.h"
#include "Game.h"
#include "RTSDemo.h"

using namespace ci;

Controls::Controls() : mCamera( NULL ), mCanSelectMultipleUnits( true )
{
	mSelectionArea.set( 0, 0, 0, 0 );
}

void Controls::setup( GameCamera* camera )
{
	mCamera = camera;
	
	mZoomStart = mCamera->getZoom();
	mRotationStart.x = mCamera->getAngle();
	mRotationStart.y = mCamera->rotation.y;
	mPositionStart = mCamera->position;
	mRotationTarget.x = mAngleEased = mCamera->getAngle();
	mRotationTarget.y = mCamera->rotation.y;
	mZoomTarget = mZoomEased = mCamera->getZoom();
	
	mOrbitSpeed = Vec2f( 0.5f, 0.5f );
	mZoomSpeed = 10.5f;
	mPanSpeed = 0.2f;
	mMoveSpeed = 0.003f;
	mPanEasing = 5.0f;
	
	mRotationEasing = 5.0f;
	mRotationXMin = -80.0f;
	mRotationXMax = 80.0f;
	
	mZoomEasing = 5.0f;
	mZoomMax = 5000.0f;
	mZoomMin = 80.0f;
	
	TouchInput::get()->addDelegate( this );
	
	mUnhighlightTimer = ly::Timer( boost::bind( &Controls::unhighlightUnit, this, boost::arg<1>() ), 0.5f, 1 );
}

void Controls::update( const float deltaTime )
{
	TouchInput* touch = TouchInput::get();
	
	// Calculate two-finger controls for rotate, zoom
	if ( touch->getTouchCount() == 2  ) {
		const Vec2f diff = touch->getTouchesDifference();
		mRotationTarget = Vec2f( mRotationStart.x - diff.y * mOrbitSpeed.y, mRotationStart.y - diff.x * mOrbitSpeed.x );
		mRotationTarget.x = math<float>::clamp( mRotationTarget.x, mRotationXMin, mRotationXMax );
		
		mZoomTarget = ci::math<float>::clamp( mZoomStart - touch->getTouchesDistance() * mZoomSpeed, mZoomMin, mZoomMax );
	}
	else {
		mRotationStart.x = mCamera->getAngle();
		mRotationStart.y = mCamera->rotation.y;
		
		mZoomStart = mCamera->getZoom();
	}
	
	// Three-finger control for panning
	if ( touch->getTouchCount() == 3  ) {
		const Vec3f direction = Vec3f( touch->getTouchesDifference().x, 0.0f, touch->getTouchesDifference().y );
		const Vec3f target = mPositionStart - mCamera->getTransform().transformVec( direction ) * mMoveSpeed * mCamera->getZoom();
		mCamera->position += ( target - mCamera->position ) / mPanEasing;
	}
	else {
		mPositionStart = mCamera->position;
	}
	
	if ( mCanSelectMultipleUnits ) {
		std::vector<Unit*>& units = RTSDemo::get()->getUnits();
		for( auto unit : units ) {
			if ( unit->factionId == 0 ) {
				unit->mIsSelected = mSelectionArea.contains( unit->getScreenPosition() );
			}
		}
	}
	
	// Apply target values with easing
	mCamera->rotation.y += ( mRotationTarget.y - mCamera->rotation.y ) / mRotationEasing;
	mAngleEased += (mRotationTarget.x - mAngleEased ) / mRotationEasing;
	mCamera->setAngle( mAngleEased );
	mZoomEased += (mZoomTarget - mZoomEased ) / mZoomEasing;
	mCamera->setZoom( mZoomEased );
	
	mUnhighlightTimer.update( deltaTime );
}

void Controls::updateSelectedUnits()
{
	mSelectedUnits.clear();
	for( const auto unit : RTSDemo::get()->getUnits() ) {
		if ( unit->mIsSelected ) {
			mSelectedUnits.push_back( unit );
		}
	}
}

void Controls::tapDown( ci::Vec2i position )
{
	Unit* singleSelectedUnit = NULL;
	
	// Test ray intersection against all units to find a single unit
	ci::Ray ray = mCamera->rayIntoScene( position );
	std::vector<Unit*>& units = RTSDemo::get()->getUnits();
	std::sort( units.begin(), units.end(), Unit::sortByDistanceFromCamera );
	for( auto unit : units ) {
		if ( unit->getNode()->getBoundingBox().intersects( ray ) ) {
			singleSelectedUnit = unit;
			break;
		}
	}
	
	// If a single unit was selected
	if ( singleSelectedUnit != NULL ) {
		
		// If the newly selected unit is an enemy
		if ( singleSelectedUnit->factionId != 0 ) {
			
			// If one or more player units are already selected, attack that unit
			if ( !mSelectedUnits.empty() ) {
				for( auto unit : mSelectedUnits ) {
					if ( unit->factionId == 0 ) {
						unit->commandAttackTarget( singleSelectedUnit );
					}
				}
				
				// Show the enemy unit as 'highlighted', meaning it will appear selected and then unselect after an interval
				singleSelectedUnit->mIsSelected = true;
				mHighlightedUnit = singleSelectedUnit;
				mUnhighlightTimer.start();
			}
		}
		else {
			unselectAllUnits();
			singleSelectedUnit->mIsSelected = true;
			updateSelectedUnits();
			mCanSelectMultipleUnits = false;
		}
	}
	// If no unit was selected
	else {
		// If units were already selected, then move selected units to the position where the tap occurred
		if ( !mSelectedUnits.empty() ) {
			float distance;
			ray.calcPlaneIntersection( Vec3f::zero(), Vec3f::yAxis(), &distance );
			for( auto unit : units ) {
				if ( unit->factionId == 0 && unit->mIsSelected ) {
					unit->commandMove( ray.getOrigin() + ray.getDirection() * distance );
				}
			}
		}
		mCanSelectMultipleUnits = true;
	}
}

void Controls::unselectAllUnits()
{
	std::vector<Unit*>& units = RTSDemo::get()->getUnits();
	for( auto unit : units ) {
		unit->mIsSelected = false;
	}
	updateSelectedUnits();
}

void Controls::unhighlightUnit( const float deltaTime )
{
	if ( mHighlightedUnit != NULL ) {
		mHighlightedUnit->mIsSelected = false;
	}
}

void Controls::tapUp( ci::Vec2i position )
{
	mCanSelectMultipleUnits = false;
}

void Controls::gestureStarted( int fingerCount )
{
	Vec2i p = TouchInput::get()->getTouchesCenter();
	if ( mCanSelectMultipleUnits && fingerCount == 1 ) {
		mSelectionArea.set( p.x, p.y, p.x, p.y );
		mCanSelectMultipleUnits = true;
	}
}

void Controls::gestureMoved( int fingerCount )
{
	Vec2i p = TouchInput::get()->getTouchesCenter();
	if ( mCanSelectMultipleUnits && fingerCount == 1 ) {
		mSelectionArea.x2 = p.x;
		mSelectionArea.y2 = p.y;
	}
}

void Controls::gestureEnded( int fingerCount )
{
	if ( fingerCount == 1 ) {
		mSelectionArea.set( 0, 0, 0, 0 );
		mCanSelectMultipleUnits = false;
		updateSelectedUnits();
	}
}


