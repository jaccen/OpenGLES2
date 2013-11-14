#include "RacingDemo.h"
#include "Scene.h"
#include "TouchInput.h"
#include "TargetConditionals.h"
#include "ProjectileManager.h"
#include "Planet.h"
#include "Game.h"

using namespace ci;

extern void onGameLaunch( Game* game )
{
	Game::get()->addController( RacingDemo::get() );
	RacingDemo::get()->setup();
}

RacingDemo* RacingDemo::sInstance = NULL;

RacingDemo* RacingDemo::get()
{
	if ( sInstance == NULL ) {
		sInstance = new RacingDemo();
	}
	return sInstance;
}

void RacingDemo::setup()
{
	mScene = Scene::get();
	Game::get()->addScene( mScene );
	
	mRenderingEngine = RenderingEngine::get();
	mResourceManager = ResourceManager::get();
	
	mResourceManager->loadShader( "environment",	"shaders/environment.vert",			"shaders/environment.frag" );
	mResourceManager->loadShader( "depth_map",		"shaders/depth_map.vert",			"shaders/depth_map.frag" );
	mResourceManager->loadShader( "screen",			"shaders/screen_space.vert",		"shaders/screen_space.frag" );
	mResourceManager->loadShader( "debug",			"shaders/debug.vert",				"shaders/debug.frag" );
	mResourceManager->loadShader( "debug_screen",	"shaders/screen_space.vert",		"shaders/debug_screen.frag" );
	mResourceManager->loadShader( "planet",			"shaders/planet.vert",				"shaders/planet.frag" );
	mResourceManager->loadShader( "level",			"shaders/level.vert",				"shaders/level.frag" );
	mResourceManager->loadShader( "unlit",			"shaders/unlit.vert",				"shaders/unlit.frag" );
	mResourceManager->loadShader( "ship",			"shaders/ship.vert",				"shaders/ship.frag" );
	mResourceManager->loadShader( "text",			"shaders/text.vert",				"shaders/text.frag" );
	mResourceManager->loadShader( "screen_text",	"shaders/screen_space.vert",		"shaders/screen_space_text.frag" );
	
	mResourceManager->loadMesh( "models/sphere_globe.obj" );
	mResourceManager->loadMesh( "models/skybox.obj" );
	mResourceManager->loadMesh( "models/skybox_unwrapped.obj" );
	mResourceManager->loadMesh( "models/quad_plane.obj" );
	
	GameCamera* camera = GameCamera::get();
	Vec2i screenSize = Game::get()->getScreenSize();
	camera->setScreenSize( screenSize.x, screenSize.y, Game::get()->getContentScaleFactor() );
	camera->setZoom( 200.0f );
	camera->setAngle( -20.0f );
	camera->setFov( 70.0f );
	camera->setBodyOffset( 0, 70 );
	camera->setRange( 10.0f, 10000.0f );
	
	mScene->addCamera( camera );
	
	Node* skyBox = new Node();
	skyBox->setMesh( mResourceManager->getMesh( "models/skybox_unwrapped.obj" ) );
	skyBox->scale = Vec3f::one() * 1000.0f;
	skyBox->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/skybox_day2.png" ) );
	skyBox->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
	skyBox->mMaterial.mShader = mResourceManager->getShader( "unlit" );
	skyBox->mMaterial.setProperty( "Scale",	Vec2f( 1.0f, 1.0f ) );
	mScene->setSkyboxNode( skyBox );
	
	Node* road = new Node();
	road->mLayer = Node::LayerObjects;
	road->setMesh( mResourceManager->getMesh( "models/plane_flat.obj" ) );
	road->mMaterial.mShader = mResourceManager->getShader( "level" );
	road->mMaterial.setTexture( "DiffuseTexture",		mResourceManager->getTexture( "textures/road_diffuse.png" ) );
	road->mMaterial.setTexture( "SpecularMapTexture",	mResourceManager->getTexture( "textures/road_spec.png" ) );
	road->mMaterial.setColor( "DiffuseMaterial",		ColorA( 1, 1, 1, 1 ) );
	road->mMaterial.setColor( "SpecularMaterial",		ColorA( 1, 1, 1, 0.5f ));
	road->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.5, 0.5, 1.0f ));
	road->mMaterial.setProperty( "Shininess", 10.0f );
	road->mMaterial.setProperty( "Scale",	Vec2f( 1.0f, 20.0f ) );
	road->scale = Vec3f( 100, 100, 2000 ) * 2.0f;
	mScene->addNode( road );
	
	std::vector<std::string> faceFilePaths;
	faceFilePaths.push_back( "textures/cube_sample_pos_x.png" );
	faceFilePaths.push_back( "textures/cube_sample_neg_x.png" );
	faceFilePaths.push_back( "textures/cube_sample_pos_y.png" );
	faceFilePaths.push_back( "textures/cube_sample_neg_y.png" );
	faceFilePaths.push_back( "textures/cube_sample_pos_z.png" );
	faceFilePaths.push_back( "textures/cube_sample_neg_z.png" );
	mResourceManager->loadCubemapTexture( "cubemap_sample", faceFilePaths );
	Texture* cubemap = mResourceManager->getTexture( "cubemap_sample" );
	
	mVehicle = new Node();
	mVehicle->mLayer = Node::LayerObjects;
	mVehicle->rotation.y = 90.0f;
	mVehicle->position.x = 80.0f;
	mVehicle->setMesh( mResourceManager->getMesh( "models/teapot.obj" ) );
	mVehicle->mMaterial.mShader = mResourceManager->getShader( "environment" );
	mVehicle->mMaterial.setTexture( "DiffuseTexture",		mResourceManager->getTexture( "textures/road_diffuse.png" ) );
	mVehicle->mMaterial.setColor( "DiffuseMaterial",		ColorA(1,0,0,1) );
	mVehicle->mMaterial.setColor( "SpecularMaterial",		ColorA::white() * 0.8f );
	mVehicle->mMaterial.setTexture( "EnvironmentMap", cubemap );
	mVehicle->mMaterial.setProperty( "Shininess", 120.0f );
	mVehicle->mMaterial.setProperty( "Reflectiveness", 0.5f );
	mVehicle->mMaterial.setProperty( "Scale",	Vec2f( 1.0f, 20.0f ) );
	mVehicle->scale = Vec3f::one() * 100.f;
	mScene->addNode( mVehicle );
	
	Node* grass = new Node();
	grass->mLayer = Node::LayerObjects;
	grass->setMesh( mResourceManager->getMesh( "models/plane_flat.obj" ) );
	grass->mMaterial.mShader = mResourceManager->getShader( "level" );
	grass->mMaterial.setTexture( "DiffuseTexture",		mResourceManager->getTexture( "textures/grass.png" ) );
	grass->mMaterial.setColor( "DiffuseMaterial",		ColorA( 1, 1, 1, 1 ) );
	grass->mMaterial.setColor( "SpecularMaterial",		ColorA( 1, 1, 1, 0.5f ));
	grass->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.5, 0.5, 1.0f ));
	grass->mMaterial.setProperty( "Shininess", 10.0f );
	grass->mMaterial.setProperty( "Scale",	Vec2f( 100.0f, 100.0f ) );
	grass->scale = Vec3f::one() * 4000.0f;
	grass->position.y = -1.0f;
	mScene->addNode( grass );
	
	Light* light = new Light();
	light->mColor = ColorA::white();
	light->mAmbientColor = ColorA::white() * 0.3f;
	light->mNode.position = Vec3f( 500, 500, 0 );
	mScene->addLight( light );
	
	mControls.setup( camera );
	
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(0.48,.35,.22,0.5),	Vec2i( 200, 200 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_0.png" ), ColorA(1,1,1,1),			Vec2i( 600, 600 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(1,.2,.2,.5),			Vec2i( 50, 50 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_2.png" ), ColorA(1,1,1,.5),			Vec2i( 20, 20 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(1,.2,.2,.5),			Vec2i( 100, 100 ) * 2.0f );
	/*light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(.2,.2,1,.5),		Vec2i( 80, 80 ) );
	 light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(.2,.2,1,.5),			Vec2i( 30, 30 ) );
	 light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_3.png" ), ColorA(1,1,1,.25),			Vec2i( 200, 200 ) );
	 light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_4.png" ), ColorA(1,1,1,.1),			Vec2i( 300, 300 ) );*/

}

RacingDemo::~RacingDemo()
{
	
}

void RacingDemo::update( const float deltaTime )
{
	mVehicle->position.z -= 50.0f * deltaTime;
	GameCamera::get()->position = mVehicle->position;
	
	mControls.update( deltaTime );
}

void RacingDemo::debugDraw()
{
	if ( false ) {
		const std::list<Node*>& nodes = mScene->getObjectNodes();
		for( auto node : nodes ) {
			mRenderingEngine->debugDrawCube( node->getBoundingBox().getCenter(), node->getBoundingBox().getSize(), ci::ColorA( 0, 1, 0, 1 ), mScene );
		}
	}
	
	const float length = 100.0f;
	mRenderingEngine->debugDrawLine( Vec3f::zero(), Vec3f::xAxis() * length, ColorA( 1, 0, 0, 1 ), mScene );
	mRenderingEngine->debugDrawLine( Vec3f::zero(), Vec3f::yAxis() * length, ColorA( 0, 1, 0, 1 ), mScene );
	mRenderingEngine->debugDrawLine( Vec3f::zero(), Vec3f::zAxis() * length, ColorA( 0, 0, 1, 1 ), mScene );
}