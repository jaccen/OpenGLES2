#include "Game.h"
#include "TouchInput.h"
#include "TargetConditionals.h"
#include "Projectile.h"

using namespace ci;

Game::Game(RenderingEngine* renderingEngine ) :
	mRenderingEngine(renderingEngine),
	mPlanet(NULL)
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
	
	mResourceManager->loadShader( "screen",			"shaders/screen_space.vert",		"shaders/screen_space.frag" );
	mResourceManager->loadShader( "debug",			"shaders/debug.vert",				"shaders/debug.frag" );
	mResourceManager->loadShader( "planet",			"shaders/planet.vert",				"shaders/planet.frag" );
	mResourceManager->loadShader( "body",			"shaders/body.vert",				"shaders/body.frag" );
	mResourceManager->loadShader( "unlit",			"shaders/unlit.vert",				"shaders/unlit.frag" );
	mResourceManager->loadShader( "ship",			"shaders/ship.vert",				"shaders/ship.frag" );
	mResourceManager->loadShader( "text",			"shaders/text.vert",				"shaders/text.frag" );
	mResourceManager->loadShader( "screen_text",	"shaders/screen_space.vert",		"shaders/screen_space_text.frag" );
	
	mResourceManager->loadMesh( "models/sphere_globe.obj" );
	mResourceManager->loadMesh( "models/skybox.obj" );
	mResourceManager->loadMesh( "models/skybox_unwrapped.obj" );
	mResourceManager->loadMesh( "models/quad_plane.obj" );
	
	mCamera = Camera::get();
	mCamera->setZoom( 200.0f );
	mCamera->setFov( 60.0f );
	mCamera->setAngle( -30.0f );
	mCamera->rotation.y = 90;
	mCamera->setRange( 10.0f, 1500.0f );
	
#if TARGET_IPHONE_SIMULATOR
#else
	mCameraController = EditorCamera( mCamera );
#endif
	
	Node* skyBox = new Node();
	skyBox->mMesh = mResourceManager->getMesh( "models/skysphere.obj" );
	skyBox->scale = Vec3f::one() * 600.0f;
	skyBox->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/stars2.jpg" ) );
	skyBox->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
	skyBox->mMaterial.mShader = mResourceManager->getShader( "unlit" );
	skyBox->mMaterial.setProperty( "Scale",	Vec2f( 2.0f, 2.0f ) );
	mRenderingEngine->setSkyboxNode( skyBox );
	
	mPlanet = new Node();
	mPlanet->mLayer = Node::LayerObjects;
	mPlanet->mMesh = mResourceManager->getMesh( "models/sphere_high.obj" );
	//mPlanet->scale = Vec3f::one() * 20.0f;
	mPlanet->scale = Vec3f::one() * 200.0f;
	mPlanet->position = Vec3f( 600, 0, 0 );
	mPlanet->mMaterial.mShader = mResourceManager->getShader( "planet" );
	mPlanet->mMaterial.setTexture( "DiffuseTexture",		mResourceManager->getTexture( "textures/mars_diffuse.png" ) );
	mPlanet->mMaterial.setColor( "DiffuseMaterial",			ColorA( 1, 1, 1, 1 ) );
	mPlanet->mMaterial.setColor( "SpecularMaterial",		ColorA( 1, 1, 1, 0.5f ));
	mPlanet->mMaterial.setColor( "RimMaterial",				ColorA( 0.35, 0.1, 0, 0.5 ) );
	mPlanet->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.2, 0.0, 1.0f ));
	mPlanet->mMaterial.setProperty( "RimPower", 0.35f );
	mPlanet->mMaterial.setProperty( "Shininess", 10.0f );
	mRenderingEngine->addNode( mPlanet );
	
	Node* glowSprite = new Node();
	glowSprite->mLayer = Node::LayerLighting;
	glowSprite->mFaceCamera = true;
	glowSprite->mMesh = mResourceManager->getMesh( "models/quad_plane.obj" );
	glowSprite->scale = mPlanet->scale * 2.6;
	glowSprite->position = mPlanet->position;
	glowSprite->mMaterial.mShader = mResourceManager->getShader( "unlit" );
	glowSprite->mMaterial.setProperty( "Scale",	Vec2f::one() );
	glowSprite->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/sphere_glow.png" ) );
	glowSprite->mMaterial.setColor( "DiffuseMaterial", ColorA( 1, .5, 0, 0.4 ) );
	mRenderingEngine->addSpriteNode( glowSprite );
		
	Projectile* proj = new Projectile();
	proj->mNode.mLayer = Node::LayerLighting;
	proj->mNode.mFaceCamera = true;
	proj->mNode.mMesh = mResourceManager->getMesh( "models/quad_plane.obj" );
	proj->mNode.scale = Vec3f( 10.0f, 1.0f, 1.0f ) * 20.0f;
	proj->mNode.mMaterial.mShader = mResourceManager->getShader( "unlit" );
	proj->mNode.mMaterial.setProperty( "Scale", Vec2f::one() );
	proj->mNode.mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/projectile.png" ) );
	proj->mNode.mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
	
	Node* asteroid = new Node();
	asteroid->mLayer = Node::LayerObjects;
	asteroid->mMesh = mResourceManager->getMesh( "models/sphere_high.obj" );
	asteroid->mMaterial.mShader = mResourceManager->getShader( "body" );
	asteroid->mMaterial.setTexture( "DiffuseTexture",		mResourceManager->getTexture( "textures/asteroid.png" ) );
	asteroid->mMaterial.setTexture( "SpecularMapTexture",	mResourceManager->getTexture( "textures/asteroid_spec.png" ) );
	asteroid->mMaterial.setColor( "DiffuseMaterial",		ColorA( 1, 1, 1, 1 ) );
	asteroid->mMaterial.setColor( "SpecularMaterial",		ColorA( 1, 1, 1, 0.5f ));
	asteroid->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.5, 0.5, 1.0f ));
	asteroid->mMaterial.setProperty( "Shininess", 10.0f );
	asteroid->scale = Vec3f::one() * 50.0f;
	asteroid->position = Vec3f( 300, 50, 0 );
	mRenderingEngine->addNode( asteroid );
	
	for( int i = 0; i < 5; i++ ) {
		Node* ship = new Node();
		ship->mLayer = Node::LayerObjects;
		ship->mMesh = mResourceManager->getMesh( "models/teapot_low.obj" );
		ship->mMaterial.mShader = mResourceManager->getShader( "ship" );
		ship->mMaterial.setTexture( "DiffuseTexture",	mResourceManager->getTexture( "textures/metal.png" ) );
		ship->mMaterial.setTexture( "SelfIlluminationTexture",	mResourceManager->getTexture( "textures/ship_selfillum.png" ) );
		ship->mMaterial.setColor( "DiffuseMaterial",		ColorA( 1, 1, 1, 1 ) );
		ship->mMaterial.setColor( "SpecularMaterial",		ColorA( 1, 1, 1, 0.5f ));
		ship->mMaterial.setColor( "SelfIlluminationColor",	ColorA( 1, 0, 0, 1.0 ) );
		ship->mMaterial.setColor( "RimMaterial",			ColorA( 0.35, 0.1, 0, 0.5 ) );
		ship->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.5, 0.5, 1.0f ));
		ship->mMaterial.setProperty( "Shininess", 10.0f );
		ship->scale = Vec3f::one() * 50.0f;
		ship->position = Vec3f( randFloat(-1.0f,1.0f) * 500.0f, 0.0f, randFloat(-1.0f,1.0f) * 500.0f );
		mRenderingEngine->addNode( ship );
	}
	
	ParticleSystem* ps = new ParticleSystem();
	ps->createParticles( 10, mResourceManager->getTexture( "textures/explosion.png" ) );
	ps->setVelocity( Vec3f::yAxis() * 100.0f, Vec3f::one() );
	ps->setRotationalVelocity( 0.0f, 100.0f );
	ps->setStartPositionRange( Vec3f::one() * 100.0f );
	
	//mRenderingEngine->setBackgroundTexture( mResourceManager->getTexture( "textures/stars2.jpg" ) );
	
	Light* light = new Light();
	light->mColor = ColorA::white();
	light->mAmbientColor = ColorA( 0.1f, 0.1f, 0.1f, 1.0f );
	light->mNode.position = Vec3f( 10000, 10000, 0 );
	mRenderingEngine->addLight( light );
	
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(0.48,.35,.22,0.5),	Vec2i( 200, 200 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_0.png" ), ColorA(1,1,1,1),			Vec2i( 600, 600 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(1,.2,.2,.5),		Vec2i( 50, 50 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_2.png" ), ColorA(1,1,1,.5),			Vec2i( 20, 20 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(1,.2,.2,.5),		Vec2i( 100, 100 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(.2,.2,1,.5),		Vec2i( 80, 80 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(.2,.2,1,.5),		Vec2i( 30, 30 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_3.png" ), ColorA(1,1,1,.25),			Vec2i( 200, 200 ) );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_4.png" ), ColorA(1,1,1,.1),			Vec2i( 300, 300 ) );
	
	return;
	
	mRootGui = new Node2d();
	mRenderingEngine->addNode( mRootGui );
	
	Node2d* test = new Node2d();
	test->name = "test_node";
	test->setColor( ColorA(1,1,1,1) );
	test->setTexture( mResourceManager->getTexture( "textures/gui_test.png" ) );
	test->position = Vec2f( 100, 100 );
	test->size = Vec2f( 100, 100 );
	mRootGui->addChild( test );
	
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

#if TARGET_IPHONE_SIMULATOR
float ___angle = 0.0f;
float ___angleZ = 0.0f;
#endif

void Game::update( const float deltaTime )
{
#if TARGET_IPHONE_SIMULATOR
	float target = math<float>::sin( ___angle += 0.025f ) * 80.0f;
	float targetZ = math<float>::sin( ___angleZ += 0.05f ) * 250.0f;
	mCamera->setAngle( target );
	mCamera->setZoom( 350.0f + targetZ );
	mCamera->rotation.y += 20.0f * deltaTime;
#else
	mCameraController.update( deltaTime );
#endif
	mCamera->update( deltaTime );
	
	if ( mPlanet ) {
		mPlanet->rotation.y += 4.0f * deltaTime;
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


