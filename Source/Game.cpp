#include "Game.h"

using namespace ci;
    
Game* CreateGame(RenderingEngine* renderingEngine, ResourceManager* resourceManager)
{
    return new Game(renderingEngine, resourceManager);
}

Game::Game(RenderingEngine* renderingEngine, ResourceManager* resourceManager) :
    mRenderingEngine(renderingEngine),
    mResourceManager(resourceManager)
{
}

Game::~Game()
{
    delete mRenderingEngine;
}

void Game::setup( int width, int height )
{
    mScreenSize = ci::Vec2i( width, height );
	
	// Initialize render engine
    mRenderingEngine->setup( width, height );
	
	// Load the shaders
	mRenderingEngine->addShader(kShaderFragmentLighting,
								 mResourceManager->loadShader( "shaders/pixel_lighting.vert" ),
								mResourceManager->loadShader( "shaders/pixel_lighting_plus.frag" ) );
	
	mRenderingEngine->addShader(kSahderVertexLighting,
								mResourceManager->loadShader( "shaders/vertex_lighting.vert" ),
								mResourceManager->loadShader( "shaders/vertex_lighting.frag" ) );
	
	mRenderingEngine->addShader(kShaderGui2d,
								mResourceManager->loadShader( "shaders/gui.vert" ),
								mResourceManager->loadShader( "shaders/gui.frag" ) );
	
	std::string path = mResourceManager->getResourcePath();
	
	Texture* texture = mResourceManager->loadImage( "textures/mars_diffuse.png" );
	mRenderingEngine->createTexture( texture );
	Texture* textureNormal = mResourceManager->loadImage( "textures/mars_normal.jpg" );
	mRenderingEngine->createTexture( textureNormal );
	
	// Create VBOs for our geometry
	mPlanet					= new Node();
	ObjParser* model		= new ObjParser( path + "/models/sphere_globe.obj");
	mPlanet->mMesh			= mRenderingEngine->createVbo( model );
    mPlanet->mColorSpecular	= ci::Vec4f( .5, .5, .5, 1.0 );
	mPlanet->mShininess		= 50.0f;
	mPlanet->mShader		= kShaderFragmentLighting;
	mPlanet->mTexture		= texture;
	mPlanet->mTextureNormal = textureNormal;
	mPlanet->scale			= Vec3f::one() * 0.6f;
	delete model;
		
	mRenderingEngine->addNode( mPlanet );
	mNodes.push_back( mPlanet );
	
	model = new ObjParser( path + "/models/tower.obj");
	texture = mResourceManager->loadImage( "textures/metal.png" );
	mRenderingEngine->createTexture( texture );
	VboMesh towerMesh = mRenderingEngine->createVbo( model );
	delete model;
	
	for( int i = 0; i < 10; i++ ) {
		Node* tower				= new Node();
		tower->mMesh			= towerMesh;
		tower->mColorSpecular	= ci::Vec4f( 0.5, 0.5, 0.5, 1.0 );
		tower->mShininess		= 100.0f;
		tower->mShader			= kSahderVertexLighting;
		tower->rotation.x		= arc4random() % 360;
		tower->rotation.y		= arc4random() % 360;
		tower->rotation.z		= arc4random() % 360;
		tower->scale			= Vec3f::one() * 2.0f;
		tower->pivotOffset.y	= 1.0f;
		tower->mTexture			= texture;
		tower->mParent			= mPlanet;
		
		mRenderingEngine->addNode( tower );
		mNodes.push_back( tower );
	}
	
	model = new ObjParser( path + "/models/quad_plane.obj");		
	mRootGui = new Gui();
	mRootGui->getNode()->mMesh = mRenderingEngine->createVbo( model );
	mRootGui->setTexture( texture );
	mRootGui->position = Vec2i( 400, -400 );
	mRootGui->size = Vec2i( 200, 100 );
	mRootGui->anchor = Vec2f( 0.5f, 0.5f );
	mRenderingEngine->addGuiNode( mRootGui );
	
	delete model;
}

void Game::draw() const
{	
    mRenderingEngine->draw();
}

void Game::update( const float deltaTime )
{
	//mPlanet->rotation.y += 10.0f * deltaTime;
	//mPlanet->rotation.x += 10.0f * deltaTime;
	
	const Vec3f target = Vec3f(mStartRotation.x + ( mTouchCurrent.y - mTouchStart.y ) * 0.75f,
							   mStartRotation.y + ( mTouchCurrent.x - mTouchStart.x ) * 0.75f,
							   0.0f );
	mPlanet->rotation += ( target - mPlanet->rotation ) / 20.0f;
	
	mRootGui->update( deltaTime );
	
	for( auto iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		node->update( deltaTime );
	}
}

void Game::touchEnded(ci::Vec2i location)
{
}

void Game::touchBegan(ci::Vec2i location)
{
	mStartRotation = mPlanet->rotation;
	mTouchStart = mTouchCurrent = location;
}

void Game::touchMoved(ci::Vec2i oldLocation, ci::Vec2i location)
{
	mTouchCurrent = location;
}



