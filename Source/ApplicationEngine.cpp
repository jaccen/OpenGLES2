#include "ApplicationEngine.h"

#define STRINGIFY(A)  #A
#include "../Resources/Shaders/vertex_lighting.vert"
#include "../Resources/Shaders/vertex_lighting.frag"
//#include "../Resources/Shaders/pixel_lighting.vert"
//#include "../Resources/Shaders/pixel_lighting.frag"

using namespace ci;

const static std::string kVertexLitShaderKey		= "vertex_lit";
    
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
    m_renderingEngine->setup( width, height );
	m_renderingEngine->addShader( kVertexLitShaderKey, SimpleVertexShader, SimpleFragmentShader );
	
	// Create VBOs for our geometry
	std::string path		= m_resourceManager->GetResourcePath();
	ObjSurface* model		= new ObjSurface(path + "/models/cube_smooth.obj");
	
	mNode.mMesh				= m_renderingEngine->createVbo( model );
    mNode.mColor			= ci::Vec4f(0.25, 0.75, 1, 1.0f);
    mNode.mSpecularColor	= ci::Vec4f( 0.5, 0.5, 0.5, 1.0 );
	mNode.mTransform		= ci::Matrix44f::identity();
	mNode.mShininess		= 100.0f;
	mNode.mShader			= kVertexLitShaderKey;
	
	delete model;
	
	mNode.mTransform.scale( Vec3f::one() * 2.0f );
	
}

void ApplicationEngine::draw() const
{	
    m_renderingEngine->draw( mNode );
}

void ApplicationEngine::update( const float deltaTime )
{
	mNode.mTransform.rotate( ci::Vec3f::xAxis(), -1.0f * deltaTime );
	mNode.mTransform.rotate( ci::Vec3f::yAxis(), 0.5f * deltaTime );
	mNode.mTransform.rotate( ci::Vec3f::zAxis(), -0.25f * deltaTime );
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
