#include "Game.h"
#include "TouchInput.h"

using namespace ci;

Game::Game(RenderingEngine* renderingEngine ) :
	mRenderingEngine(renderingEngine),
	mPlanet(NULL),
	mLensFlare(NULL)
{
}

Game::~Game()
{
    delete mRenderingEngine;
}

void Game::remove( Node2d* Node2d ) {}

void Game::setup( int width, int height )
{
	mResourceManager = ResourceManager::get();
	mResourceManager->setup( mRenderingEngine );
	
	mResourceManager->loadShader( kShaderText,				"shaders/text.vert",				"shaders/text.frag" );
	mResourceManager->loadShader( kShaderFragmentLighting,	"shaders/planet.vert",				"shaders/planet.frag" );
	mResourceManager->loadShader( kSahderClouds,			"shaders/pixel_lighting.vert",		"shaders/clouds.frag" );
	mResourceManager->loadShader( kSahderVertexLighting,	"shaders/vertex_lighting.vert",		"shaders/vertex_lighting.frag" );
	mResourceManager->loadShader( kShaderVertexLightColor,	"shaders/vertex_lighting.vert",		"shaders/color.frag" );
	mResourceManager->loadShader( kShaderUnlit,				"shaders/unlit.vert",				"shaders/unlit.frag" );
	mResourceManager->loadShader( kShaderScreenSpace,		"shaders/screen_space.vert",		"shaders/screen_space.frag" );
	mResourceManager->loadShader( kShaderScreenSpaceBW,		"shaders/screen_space.vert",		"shaders/black_and_white.frag" );
	mResourceManager->loadShader( kShaderDebug,				"shaders/debug.vert",				"shaders/debug.frag" );
	mResourceManager->loadShader( kShaderShip,				"shaders/ship.vert",				"shaders/ship.frag" );
	mResourceManager->loadShader( kShaderScreenText,		"shaders/screen_space.vert",		"shaders/screen_space_text.frag" );
	
	mResourceManager->loadMesh( "models/sphere_globe.obj" );
	mResourceManager->loadMesh( "models/skybox.obj" );
	mResourceManager->loadMesh( "models/skybox_unwrapped.obj" );
	mResourceManager->loadMesh( "models/quad_plane.obj" );
	
	mCamera = Camera::get();
	mCamera->setZoom( 100.0f );
	mCamera->setFov( 60.0f );
	mCamera->setAngle( -30.0f );
	mCamera->rotation.y = 90;
	
	//mCameraController		= EditorCamera( mCamera );
	
	mPlanet = new Node();
	mPlanet->mLayer = Node::LayerObjects;
	mPlanet->mMesh = mResourceManager->getMesh( "models/sphere_high.obj" );
	mPlanet->scale = Vec3f::one() * 20.0f;
	mPlanet->mMaterial.mShader = mResourceManager->getShader( kSahderVertexLighting );
	mPlanet->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/mars_diffuse.png" ) );
	mPlanet->mMaterial.setColor( "DiffuseMaterial", ColorA( 1, 1, 1, 1 ) );
	mPlanet->mMaterial.setColor( "SpecularMaterial", ColorA( 1, 1, 1, 0.5f ));
	mPlanet->mMaterial.setColor( "AmbientMaterial", ColorA( 0.1, 0.1, 0.1, 1.0f ));
	//mPlanet->mMaterial.setTexture( "NormalTexture", mResourceManager->getTexture( "textures/mars_normal.jpg" ) );
	//mPlanet->mMaterial.setColor( "SelfIlluminationColor", ColorA( 1, 0, 0, 1.0 ) );
	//mPlanet->mMaterial.setColor( "RimColor", ColorA( 1, 0, 0, 1.0 ) );
	//mPlanet->mMaterial.setColor( "SpecularColor", ColorA( 1, 0, 0, 1.0 ) );
	mPlanet->mMaterial.setProperty( "RimPower", 0.15f );
	mPlanet->mMaterial.setProperty( "Shininess", 10.0f );
	mRenderingEngine->addNode( mPlanet );
	
	Node* glowSprite = new Node();
	glowSprite->mLayer = Node::LayerLighting;
	glowSprite->mFaceCamera = true;
	glowSprite->mMesh = mResourceManager->getMesh( "models/quad_plane.obj" );
	glowSprite->scale = Vec3f::one() * 50.0f;
	mRenderingEngine->addSpriteNode( glowSprite );
	glowSprite->mMaterial.mShader = mResourceManager->getShader( kShaderUnlit );
	glowSprite->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/sphere_glow.png" ) );
	glowSprite->mMaterial.setColor( "DiffuseMaterial", ColorA( 1, .5, 0, 0.4 ) );
	glowSprite->mMaterial.setProperty( "rimPower", 0.15f );
	
	Node* skyBox = new Node();
	skyBox->mMesh = mResourceManager->getMesh( "models/skysphere.obj" );
	skyBox->scale = Vec3f::one() * 200.0f;
	skyBox->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/stars.jpg" ) );
	skyBox->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
	skyBox->mMaterial.mShader = mResourceManager->getShader( kShaderUnlit );
	mRenderingEngine->setSkyboxNode( skyBox );
	
	//mRenderingEngine->setBackgroundTexture( mResourceManager->getTexture( "textures/stars2.jpg" ) );
	
	mLensFlare = new LensFlare();
	mLensFlare->setLightPosition( Vec3f( 3000, 4000, 0 ) );
	
	mRootGui = new Node2d();
	mRenderingEngine->addNode( mRootGui );
	
	Node2d* test = new Node2d();
	test->name = "test_node";
	test->setColor( ColorA(1,1,1,1) );
	test->setTexture( mResourceManager->getTexture( "textures/gui_test.png" ) );
	test->position = Vec2f( 100, 100 );
	test->size = Vec2f( 100, 100 );
	mRootGui->addChild( test );
	
	return;
	
	Font* font = ResourceManager::get()->getFont( "fonts/menlo.fnt" );
	Node2d* child = new Node2d();
	Text::Options opts;
	opts.scale = 0.4f;
	opts.padding = 25;
	opts.width = 256;
	opts.height = 150;
	opts.letterSpacing = 2;
	opts.color = Vec4f( 1, 1, 1, 1 );
	opts.alignment = Text::RIGHT;
	opts.truncateWithDots = true;
	child->mText = new Text( font, opts,  "\"The 14210 quick brown foxes jumped over the 21543 lazy dogs,\" I said.  \"The 1440 quick brown foxes jumped over the 5432142 lazy dogs,\" I said.  \"The 10 quick brown foxes jumped over the 52 lazy dogs,\" I said.  \"The 4320 quick brown foxes jumped over the 52 lazy dogs,\" I said." );
	child->getNode()->mLayer = Node::LayerGui;
	//Texture* t = mResourceManager->getTexture( "textures/test_dialog.png" );
	//child->setTexture( t );
	child->position = Vec2i( 500, 300 );
	child->anchor = Vec2f( 0.0f, 0.0f );
	mRootGui->addChild( child );
}

float ___angle = 0.0f;
float ___angleZ = 0.0f;

void Game::update( const float deltaTime )
{
	float target = math<float>::sin( ___angle += 0.025f ) * 80.0f;
	float targetZ = math<float>::sin( ___angleZ += 0.05f ) * 250.0f;
	mCamera->setAngle( target );
	mCamera->setZoom( 350.0f + targetZ );
	mCamera->rotation.y += 20.0f * deltaTime;
	mCamera->update( deltaTime );
	
	//mCameraController.update( deltaTime );
	
	if ( mPlanet ) {
		mPlanet->rotation.y += 4.0f * deltaTime;
	}
	
	if ( mLensFlare ) {
		mLensFlare->update( deltaTime );
	}
}

void Game::debugDraw()
{
	return;
	
	// Draw grid
	Vec4f c = Vec4f( 1, 1, 1, 0.15f );
	float l = 5.0f;
	float n = 10;
	for( float i = -n; i <= n; i++ ) {
		mRenderingEngine->debugDrawLine( Vec3f(i,0,-n) * l, Vec3f(i,0,n) * l, c );
		mRenderingEngine->debugDrawLine( Vec3f(-n,0,i) * l, Vec3f(n,0,i) * l, c );
	}
}

ObjectController* Game::pickObject( const Ray& ray )
{
	/*for( auto iter = mControllers.begin(); iter != mControllers.end(); iter++ ) {
		ObjectController* controller = *iter;
		Node* node = controller->getNode();
		
		if ( node->mLayer != Node::LayerObjects ) continue;
		if ( node->mMesh == NULL ) continue;
		if ( node == mFocusTarget ) continue;
		
		if( controller->getBoundingBox().intersects( ray ) ) {
			return controller;
		}
	}
	return NULL;*/
}


