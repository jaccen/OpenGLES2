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
	
	
	std::string path		= m_resourceManager->getResourcePath();
	
	// Create VBOs for our geometry
	Node* planet			= new Node();
	ObjSurface* model		= new ObjSurface(path + "/models/yoda.obj");
	planet->mMesh			= m_renderingEngine->createVbo( model );
    planet->mColor			= ci::Vec4f( 1, 1, 1, 1.0f );
    planet->mSpecularColor	= ci::Vec4f( 1, 1, 1, 1.0 );
	planet->mTransform		= ci::Matrix44f::identity();
	planet->mShininess		= 50.0f;
	planet->mShader			= kVertexLitShaderKey;
	planet->mTransform.translate( Vec3f::yAxis() * -1.0f );
	planet->mTransform.scale( Vec3f::one() * 1.0f );
	Texture* texture		= m_resourceManager->loadImage( "yoda" );
	m_renderingEngine->createTexture( texture );
	planet->mTexture		= texture;
	delete model;
	
	Node* tower				= new Node();
	model					= new ObjSurface(path + "/models/tower.obj");
	tower->mMesh				= m_renderingEngine->createVbo( model );
    tower->mColor			= ci::Vec4f(0.25, 0.75, 1, 1.0f);
    tower->mSpecularColor	= ci::Vec4f( 0.5, 0.5, 0.5, 1.0 );
	tower->mTransform		= ci::Matrix44f::identity();
	tower->mShininess		= 100.0f;
	tower->mShader			= kVertexLitShaderKey;
	tower->mTransform.scale( Vec3f::one() * 1.0f );
	delete model;
	
	//m_renderingEngine->addNode( tower );
	m_renderingEngine->addNode( planet );
	
	mNodes.push_back( planet );
	mNodes.push_back( tower );
}

void ApplicationEngine::draw() const
{	
    m_renderingEngine->draw();
}

void ApplicationEngine::update( const float deltaTime )
{
	for( std::list<Node*>::iterator iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		//node->mTransform.rotate( ci::Vec3f::xAxis(), -1.0f * deltaTime );
		node->mTransform.rotate( ci::Vec3f::yAxis(), 0.5f * deltaTime );
		//node->mTransform.rotate( ci::Vec3f::zAxis(), -0.25f * deltaTime );
	}
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
