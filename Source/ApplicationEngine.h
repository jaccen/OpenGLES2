#pragma once

#include "ObjSurface.h"
#include "ParametricEquations.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"

#include <list>

class ApplicationEngine {
public:
    ApplicationEngine(RenderingEngine* renderingEngine, ResourceManager* resourceManager);
    ~ApplicationEngine();
	
    void setup( int width, int height );
    void draw() const;
    void update( const float deltaTime );
	
    void touchEnded( ci::Vec2i location);
    void touchBegan( ci::Vec2i location);
    void touchMoved( ci::Vec2i oldLocation, ci::Vec2i newLocation );
	
private:
	//std::list<Node*> mNodes;
	
    RenderingEngine* m_renderingEngine;
    ResourceManager* m_resourceManager;
    
	ci::Vec2i m_screenSize;
	
	Node mNode;
};