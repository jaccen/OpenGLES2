#include "ApplicationEngine.h"
    
ApplicationEngine* CreateApplicationEngine(RenderingEngine* renderingEngine, ResourceManager* resourceManager)
{
    return new ApplicationEngine(renderingEngine, resourceManager);
}

ApplicationEngine::ApplicationEngine(RenderingEngine* renderingEngine, ResourceManager* resourceManager) :
    m_renderingEngine(renderingEngine),
    m_resourceManager(resourceManager)
{
}

ApplicationEngine::~ApplicationEngine()
{
    delete m_renderingEngine;
}

void ApplicationEngine::setup( int width, int height )
{
    m_screenSize = ivec2(width, height );
	
	std::string path = m_resourceManager->GetResourcePath();
	ObjSurface* model = new ObjSurface(path + "/micronapalmv2.obj");
	
     /*vector<ISurface*> surfaces(SurfaceCount);
   surfaces[0] = new ObjSurface(path + "/micronapalmv2.obj");
    surfaces[1] = new ObjSurface(path + "/Ninja.obj");
    surfaces[2] = new Torus(1.4, 0.3);
    surfaces[3] = new TrefoilKnot(1.8f);
    surfaces[4] = new KleinBottle(0.2f);
    surfaces[5] = new MobiusStrip(1);*/
	
    m_renderingEngine->setup();
    m_renderingEngine->createVbo( model );
}

void ApplicationEngine::draw() const
{
	Visual visual;
    visual.Color = vec3(0.25, 0.75, 1);
    visual.LowerLeft = ivec2(0, 0);
    visual.ViewportSize = ivec2(m_screenSize.x, m_screenSize.y);
	
    m_renderingEngine->draw( visual );
}

void ApplicationEngine::update( const float deltaTime )
{
}

void ApplicationEngine::touchEnded(ivec2 location)
{
}

void ApplicationEngine::touchBegan(ivec2 location)
{
}

void ApplicationEngine::touchMoved(ivec2 oldLocation, ivec2 location)
{
}
