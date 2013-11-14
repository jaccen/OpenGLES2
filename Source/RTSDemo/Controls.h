#pragma once

#include "GameCamera.h"
#include "TouchInput.h"
#include "Unit.h"
#include "Timer.h"

#include "cinder/Vector.h"

class Controls : TouchInput::IDelegate {
public:
	Controls();
	void setup( GameCamera* camera );
	virtual void update( const float deltaTime );
	
	virtual void gestureStarted( int fingerCount );
	virtual void gestureMoved( int fingerCount );
	virtual void gestureEnded( int fingerCount );
	virtual void tapDown(ci::Vec2i position);
	virtual void tapUp(ci::Vec2i position);
	
	void unselectAllUnits();
	const ci::Area& getSelectionArea() const { return mSelectionArea; }
	
	void updateSelectedUnits();
	
private:
	void unhighlightUnit( const float deltaTime );
	ly::Timer mUnhighlightTimer;
	
	std::list<Unit*> mSelectedUnits;
	bool mCanSelectMultipleUnits;
	GameCamera* mCamera;
	ci::Area mSelectionArea;
	Unit* mHighlightedUnit;
	
	ci::Vec2f mRotationStart;
	float mZoomStart;
	float mZoomSpeed;
	ci::Vec2f mOrbitSpeed;
	float mPanSpeed;
	ci::Vec3f mPositionStart;
	float mLookSensitivity;
	float mStrafeSpeed;
	float mMoveSpeed;
	float mPanEasing;
	
	ci::Vec2f mRotationTarget;
	float mRotationEasing;
	float mRotationXMax;
	float mRotationXMin;
	float mAngleEased;
	
	float mZoomTarget;
	float mZoomEased;
	float mZoomEasing;
	float mZoomMax;
	float mZoomMin;
};