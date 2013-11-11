#pragma once

#include "Game.h"

class RTSDemo : public Game::Controller {
public:
	static RTSDemo* get();
	
	void setup();
	
	virtual void update( const float deltaTime );
	virtual void debugDraw();
	
	const std::vector<Unit*>& getUnits() const { return mUnits; }
	std::vector<Unit*>& getUnits() { return mUnits; }
	
	const UniformGrid&	getUniformGrid() const { return mUniformGrid; }
	ProjectileManager&	getProjectileManager() { return mProjectileManager; }
	
	Scene* getMainScene() const { return mScene; }
	
private:
	RTSDemo() {}
	virtual ~RTSDemo();
	
	static RTSDemo* sInstance;
	
	Game* mGame;
	
	ProjectileManager		mProjectileManager;
	RenderingEngine*		mRenderingEngine;
	ResourceManager*		mResourceManager;
	
	Controls				mControls;
	
	UniformGrid				mUniformGrid;
	
	void					deselectAllControllers();
	
	std::vector<Unit*>		mUnits;
	std::list<Unit*>		mDeletionQueue;
	
	Node*					lineTest;
	
	Scene*					mScene;
};