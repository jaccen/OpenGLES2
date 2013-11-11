#pragma once

#include "Game.h"

class RacingDemo : public Game::Controller {
public:
	static RacingDemo* get();
	
	void setup();
	
	virtual void update( const float deltaTime );
	virtual void debugDraw();
	
	Scene* getMainScene() const { return mScene; }
	
private:
	RacingDemo() {}
	virtual ~RacingDemo();
	
	static RacingDemo* sInstance;
	
	Node* mVehicle;
	
	Game* mGame;
	RenderingEngine*		mRenderingEngine;
	ResourceManager*		mResourceManager;
	Scene*					mScene;
};