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
    m_screenSize = ci::Vec2i( width, height );
	
	// Create VBOs for our geometry
	std::string path = m_resourceManager->GetResourcePath();
	ObjSurface* model = new ObjSurface(path + "/models/geosphere_center.obj");
     /*surfaces[0] = new ObjSurface(path + "/micronapalmv2.obj");
    surfaces[1] = new ObjSurface(path + "/Ninja.obj");
    surfaces[2] = new Torus(1.4, 0.3);
    surfaces[3] = new TrefoilKnot(1.8f);
    surfaces[4] = new KleinBottle(0.2f);
    surfaces[5] = new MobiusStrip(1);*/
    m_renderingEngine->setup( width, height );
    mNode.mColor = ci::Vec3f(0.25, 0.75, 1);
	mNode.mMesh =  m_renderingEngine->createVbo( model );
	mNode.mTransform = ci::Matrix44f::identity();
   
	delete model;
	
}

void ApplicationEngine::draw() const
{	
    m_renderingEngine->draw( mNode );
}

void ApplicationEngine::update( const float deltaTime )
{
	mNode.mTransform.rotate( ci::Vec3f::yAxis(), 1.0f * deltaTime );
}

void ApplicationEngine::touchEnded(ci::Vec2i location)
{
}

void ApplicationEngine::touchBegan(ci::Vec2i location)
{
}

void ApplicationEngine::touchMoved(ci::Vec2i oldLocation, ci::Vec2i location)
{
}
