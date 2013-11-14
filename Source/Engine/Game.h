#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "Node.h"
#include "Node2d.h"
#include "GameConstants.h"
#include "GameCamera.h"
#include "LensFlare.h"
#include "TouchInput.h"
#include "Text.h"
#include "Scene.h"

#include <list>

extern void onGameLaunch( Game* game );

class Game {
public:
	class Controller {
	public:
		Controller() {}
		virtual void update( const float deltaTime ) = 0;
		virtual void debugDraw() {}
	};
	
	static Game*				get();
	
    void						setup( const int width, const int height, const float contentScaleFactor );
    void						update( const float deltaTime = 0.0f );
	void						draw();
	
	void						addScene( Scene* scene );
	void						removeScene( Scene* scene );
	
	RenderingEngine*			getRenderer() const { return mRenderer; }
	TouchInput*					getTouchInput() const { return mTouchInput; }
	
	const ci::Vec2i&			getScreenSize() const { return mScreenSize; }
	const float&				getContentScaleFactor() const { return mContentScaleFactor; }
	
	void						addController( Controller* controller );
	void						removeController( Controller* controller );
	
	const std::list<Scene*>&		getScenes() const { return mScenes; }
	const std::list<Controller*>&	getControllers() const { return mControllers; }
	
private:
    Game();
    ~Game();
	
	static Game*				sInstance;
	
	ci::Vec2i					mScreenSize;
	float						mContentScaleFactor;
	
	std::list<Scene*>			mScenes;
	
	RenderingEngine*			mRenderer;
	TouchInput*					mTouchInput;
	std::list<Controller*>		mControllers;
};