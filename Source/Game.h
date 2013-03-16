#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "Node.h"
#include "Gui.h"
#include "GameConstants.h"

#include <list>

class Game {
public:
    Game(RenderingEngine* renderingEngine, ResourceManager* resourceManager);
    ~Game();
	
    void setup( int width, int height );
    void draw() const;
    void update( const float deltaTime );
	
    void touchEnded( ci::Vec2i location);
    void touchBegan( ci::Vec2i location);
    void touchMoved( ci::Vec2i oldLocation, ci::Vec2i newLocation );
	
private:
	Gui* mRootGui;
	Node* mPlanet;
	std::list<Node*> mNodes;
	
    RenderingEngine* mRenderingEngine;
    ResourceManager* mResourceManager;
    
	ci::Vec2i mScreenSize;
	ci::Vec2i mTouchStart;
	ci::Vec2i mTouchCurrent;
	ci::Vec3f mStartRotation;
};