#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "Node.h"
#include "Gui.h"
#include "GameConstants.h"
#include "Camera.h"

#include <list>

class Game {
public:
    Game( RenderingEngine* renderingEngine );
    ~Game();
	
    void				setup( int width, int height );
    void				update( const float deltaTime );
	
    void				touchEnded( const std::vector<ci::Vec2i>& positions );
    void				touchBegan( const std::vector<ci::Vec2i>& positions );
    void				touchMoved( const std::vector<ci::Vec2i>& positions );
	
	Gui*				getRootGui() const { return mRootGui; }
	Camera*				getCamera() const { return mCamera; }
	ResourceManager*	getResourceManager() const { return mResourceManager; }
	RenderingEngine*	getRenderingEngine() const { return mRenderingEngine; }
	
	void				add( Node* node );
	void				add( Gui* gui );
	void				remove( Node* node );
	void				remove( Gui* gui );
	
private:
	ci::Vec2i mScreenSize;
	Camera* mCamera;
	
	Gui* mRootGui;
	Node* mPlanet;
	std::list<Node*> mNodes;
	
    RenderingEngine* mRenderingEngine;
    ResourceManager* mResourceManager;
    
	float mTouchDistanceStart;
	float mTouchDistanceCurrent;
	float mZoomStart;
	
	ci::Vec2i mTouchStart;
	ci::Vec2i mTouchCurrent;
	ci::Vec3f mStartRotation;
};