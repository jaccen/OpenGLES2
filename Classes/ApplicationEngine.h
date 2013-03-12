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
	
    void touchEnded( ivec2 location);
    void touchBegan( ivec2 location);
    void touchMoved( ivec2 oldLocation, ivec2 newLocation );
	
private:
	//std::list<Node*> mNodes;
	
    RenderingEngine* m_renderingEngine;
    ResourceManager* m_resourceManager;
    
	ivec2 m_screenSize;
};