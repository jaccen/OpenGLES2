#include "RTSDemo.h"
#include "Scene.h"
#include "TouchInput.h"
#include "TargetConditionals.h"
#include "ProjectileManager.h"
#include "Planet.h"
#include "Game.h"
#include "Audio.h"

using namespace ci;

extern void onGameLaunch( Game* game )
{
	Game::get()->addController( RTSDemo::get() );
	RTSDemo::get()->setup();
}

RTSDemo* RTSDemo::sInstance = NULL;

RTSDemo* RTSDemo::get()
{
	if ( sInstance == NULL ) {
		sInstance = new RTSDemo();
	}
	return sInstance;
}

void RTSDemo::setup()
{
	mScene = Scene::get();
	Game::get()->addScene( mScene );
	
	mRenderingEngine = RenderingEngine::get();
	mResourceManager = ResourceManager::get();
	
	mResourceManager->loadShader( "depth_map",		"shaders/depth_map.vert",			"shaders/depth_map.frag" );
	mResourceManager->loadShader( "screen",			"shaders/screen_space.vert",		"shaders/screen_space.frag" );
	mResourceManager->loadShader( "debug",			"shaders/debug.vert",				"shaders/debug.frag" );
	mResourceManager->loadShader( "debug_screen",	"shaders/screen_space.vert",		"shaders/debug_screen.frag" );
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
	
	GameCamera* camera = GameCamera::get();
	Vec2i screenSize = Game::get()->getScreenSize();
	camera->setScreenSize( screenSize.x, screenSize.y, Game::get()->getContentScaleFactor() );
	camera->setZoom( 1800.0f );
	camera->setFov( 60.0f );
	camera->setAngle( -30.0f );
	camera->rotation.y = 90;
	camera->setRange( 50.0f, 10000.0f );
	
	mScene->addCamera( camera );
	
	mControls.setup( camera );
	
	Node* skyBox = new Node();
	skyBox->setMesh( mResourceManager->getMesh( "models/skysphere.obj" ) );
	skyBox->scale = Vec3f::one() * 7000.0f;
	skyBox->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/stars2.jpg" ) );
	skyBox->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
	skyBox->mMaterial.mShader = mResourceManager->getShader( "unlit" );
	skyBox->mMaterial.setProperty( "Scale",	Vec2f( 4.0f, 2.0f ) );
	mScene->setSkyboxNode( skyBox );
	
	//mRenderingEngine->setBackgroundTexture( mResourceManager->getTexture( "textures/stars2.jpg" ) );
	
	Node* pNode = new Node();
	pNode->mLayer = Node::LayerObjects;
	pNode->setMesh( mResourceManager->getMesh( "models/sphere_high.obj" ) );
	pNode->scale = Vec3f::one() * 1200.0f;
	pNode->position = Vec3f( 0, 0, -2800 );
	pNode->rotation.y = 180.0f;
	pNode->mMaterial.mShader = mResourceManager->getShader( "planet" );
	pNode->mMaterial.setTexture( "DiffuseTexture",		mResourceManager->getTexture( "textures/mars_diffuse.png" ) );
	pNode->mMaterial.setColor( "DiffuseMaterial",			ColorA( 1, 1, 1, 1 ) );
	pNode->mMaterial.setColor( "SpecularMaterial",		ColorA( 1, 1, 1, 0.5f ));
	pNode->mMaterial.setColor( "RimMaterial",				ColorA( 0.35, 0.1, 0, 0.5 ) );
	pNode->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.2, 0.0, 1.0f ));
	pNode->mMaterial.setProperty( "RimPower", 0.15f );
	pNode->mMaterial.setProperty( "Shininess", 10.0f );
	mScene->addNode( pNode );
	__unused Planet* planet = new Planet( pNode );
	
	/*Node* glowSprite = new Node();
	 glowSprite->mLayer = Node::LayerLighting;
	 glowSprite->mFaceCamera = true;
	 glowSprite->setMesh( mResourceManager->getMesh( "models/quad_plane.obj" );
	 glowSprite->scale = pNode->scale * 2.6;
	 glowSprite->position = pNode->position;
	 glowSprite->mMaterial.mShader = mResourceManager->getShader( "unlit" );
	 glowSprite->mMaterial.setProperty( "Scale",	Vec2f::one() );
	 glowSprite->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/sphere_glow.png" ) );
	 glowSprite->mMaterial.setColor( "DiffuseMaterial", ColorA( 1, .5, 0, 0.4 ) );
	 mRenderingEngine->addSpriteNode( glowSprite );*/
	
	/*{
	 Vec3f rotations[3] = { Vec3f::zero(), Vec3f( 0, 90.0, 0 ), Vec3f( 90, 90, 0 ) };
	 for( int i = 0; i < 3; i++ ) {
	 Node* s = new Node();
	 s->mLayer = Node::LayerLighting;
	 s->setMesh( mResourceManager->getMesh( "models/quad_plane.obj" );
	 s->scale = Vec3f::one() * 100.0;
	 s->rotation = rotations[i];
	 s->mMaterial.mShader = mResourceManager->getShader( "unlit" );
	 s->mMaterial.setProperty( "Scale",	Vec2f::one() );
	 s->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/sphere_glow.png" ) );
	 s->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
	 mRenderingEngine->addSpriteNode( s );
	 }
	 }*/
	
	for( int i = 0; i < 10; i++ ) {
		Node* asteroid = new Node();
		asteroid->mLayer = Node::LayerObjects;
		asteroid->setMesh( mResourceManager->getMesh( "models/sphere_low.obj" ) );
		asteroid->mMaterial.mShader = mResourceManager->getShader( "body" );
		asteroid->mMaterial.setTexture( "DiffuseTexture",		mResourceManager->getTexture( "textures/asteroid.png" ) );
		asteroid->mMaterial.setTexture( "SpecularMapTexture",	mResourceManager->getTexture( "textures/asteroid_spec.png" ) );
		asteroid->mMaterial.setColor( "DiffuseMaterial",		ColorA( 1, 1, 1, 1 ) );
		asteroid->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.5, 0.5, 1.0f ));
		asteroid->mMaterial.setProperty( "Shininess", 10.0f );
		asteroid->scale = Vec3f::one() * ( 40.0f + randFloat() * 100.0f );
		asteroid->position = Vec3f ( 1000, 0, 1000 );
		asteroid->setParent( pNode );
		mScene->addNode( asteroid );
	}
	
	/*lineTest = new Node();
	 lineTest->mLayer = Node::LayerLighting;
	 lineTest->setMesh( mResourceManager->getMesh( "models/quad_plane.obj" );
	 lineTest->scale = Vec3f( 100, 100, 100 );
	 lineTest->rotation.y = 90.0f;
	 //lineTest->rotation = rotations[i];
	 lineTest->mFaceCameraAsLine = true;
	 //lineTest->mFaceCameraAsLine = true;
	 lineTest->mMaterial.mShader = mResourceManager->getShader( "unlit" );
	 lineTest->mMaterial.setProperty( "Scale",	Vec2f::one() );
	 lineTest->mMaterial.setTexture( "DiffuseTexture", mResourceManager->getTexture( "textures/line_test.png" ) );
	 lineTest->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
	 mRenderingEngine->addSpriteNode( lineTest );*/
	
	ColorA colors[] = {
		ColorA( .5, 1, .5, 1 ),
		ColorA( 1, .5, .5, 1 ),
		ColorA( 1, .5, 1, 1 ),
		ColorA( 1, 1, .5, 1 )
	};
	
	Vec3f startPositions[2] = { Vec3f( -1000, 0, 0 ), Vec3f( 1000, 0, 0 ) };
	for( int f = 0; f < 2; f++ ) {
		for( int i = 0; i < 15; i++ ) {
			Node* ship = new Node();
			ship->mLayer = Node::LayerObjects;
			ship->setMesh( mResourceManager->getMesh( "models/fighter_01.obj" ) );
			ship->mMaterial.mShader = mResourceManager->getShader( "ship" );
			ship->mMaterial.setTexture( "DiffuseTexture",	mResourceManager->getTexture( "textures/fighter_01.png" ) );
			ship->mMaterial.setTexture( "SelfIlluminationTexture",	mResourceManager->getTexture( "textures/ship_selfillum.png" ) );
			ship->mMaterial.setColor( "SpecularMaterial",		ColorA( 1, 1, 1, 0.5f ));
			ship->mMaterial.setColor( "SelfIlluminationColor",	ColorA( 1, 0, 0, 1.0 ) );
			ship->mMaterial.setColor( "RimMaterial",			ColorA( 0.35, 0.1, 0, 0.5 ) );
			ship->mMaterial.setColor( "SpecularMaterial",		ColorA( 0.5, 0.5, 0.5, 1.0f ));
			ship->mMaterial.setProperty( "Shininess", 10.0f );
			ship->scale = Vec3f::one() * 3.0f;
			ship->position = startPositions[f] + Vec3f( randFloat(-1.0f,1.0f), 0.0f, randFloat(-1.0f,1.0f) ) * 500;
			mScene->addNode( ship );
			
			Unit* unit = new Unit( ship );
			unit->factionId = f;
			mUnits.push_back( unit );
			
			ship->mMaterial.setColor( "DiffuseMaterial", colors[ unit->factionId ] );
		}
	}
	
	Light* light = new Light();
	light->mColor = ColorA::white();
	light->mAmbientColor = ColorA( 0.15f, 0.05f, 0.05f, 1.0f );
	light->mNode.position = Vec3f( 10000, 500, 20000 );
	mScene->addLight( light );
	
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(0.48,.35,.22,0.5),	Vec2i( 200, 200 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_0.png" ), ColorA(1,1,1,1),			Vec2i( 600, 600 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(1,.2,.2,.5),			Vec2i( 50, 50 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_2.png" ), ColorA(1,1,1,.5),			Vec2i( 20, 20 ) * 2.0f );
	light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(1,.2,.2,.5),			Vec2i( 100, 100 ) * 2.0f );
	/*light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(.2,.2,1,.5),		Vec2i( 80, 80 ) );
	 light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_1.png" ), ColorA(.2,.2,1,.5),			Vec2i( 30, 30 ) );
	 light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_3.png" ), ColorA(1,1,1,.25),			Vec2i( 200, 200 ) );
	 light->mLensFlare.addSprite( mResourceManager->getTexture( "textures/flare_sprite_4.png" ), ColorA(1,1,1,.1),			Vec2i( 300, 300 ) );*/
	
	const float t = 2000.0f;
	mUniformGrid.build( Rectf( -t, -t, t, t ), Vec2i( 10, 10 ) );
	mUniformGrid.sort( mUnits );
	
	Node2d* test = new Node2d();
	test->name = "test_node";
	test->setColor( ColorA(1,1,1,1) );
	test->setTexture( mResourceManager->getTexture( "textures/gui_test.png" ) );
	test->setTexture( mResourceManager->getTexture( "textures/gui_test_highlighted.png" ), Node2d::State::HIGHLIGHTED );
	test->position = Vec2f( 100, 100 );
	test->size = Vec2f( 100, 100 );
	mScene->addNode( test );
	
	 /*Font* font = ResourceManager::get()->getFont( "fonts/menlo.fnt" );
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
	 mRootGui->addChild( child );*/
	
	mResourceManager->loadSound( "audio/test.caf" );
	Audio::get()->playSound( mResourceManager->getSound( "audio/test.caf" ) );
}

RTSDemo::~RTSDemo()
{
	
}

void RTSDemo::update( const float deltaTime )
{
	mControls.update( deltaTime );
	mProjectileManager.update( deltaTime );
	
	for( auto u : mUnits ) {
		u->update( deltaTime );
		if ( u->getIsDead() ) {
			mDeletionQueue.push_back( u );
		}
	}
	for( auto u : mDeletionQueue ) {
		auto match = std::find( mUnits.begin(), mUnits.end(), u );
		if ( match != mUnits.end() ) {
			mScene->removeNode( (*match)->getNode() );
			delete *match;
			mUnits.erase( match );
		}
	}
	mDeletionQueue.clear();
	
	mUniformGrid.sort( mUnits );
}


void RTSDemo::debugDraw()
{
	// Draw all bounding boxes
	if ( false ) {
		const std::list<Node*>& nodes = mScene->getObjectNodes();
		for( auto node : nodes ) {
			mRenderingEngine->debugDrawCube( node->getBoundingBox().getCenter(), node->getBoundingBox().getSize(), ci::ColorA( 0, 1, 0, 1 ), mScene );
		}
	}
	
	// Draw uniform grid
	if ( false ) {
		Matrix44f m = Matrix44f::identity();
		m.rotate( Vec3f( 90.0f * kToRad, 0.0f, 0.0f ) );
		for( auto cell : mUniformGrid.getCells() ) {
			mRenderingEngine->debugDrawStrokedRect( Rectf( cell->getArea() ), cell->mDebugColor, mScene, m );
			
			for( auto unit : cell->getChildren() ) {
				std::list<Unit*> all;
				Unit* nearest = mUniformGrid.findNearest( Unit::FindVisibleEnemy( unit ) );
				if ( nearest != NULL ) {
					mRenderingEngine->debugDrawLine( unit->getNode()->position, nearest->getNode()->position, cell->mDebugColor, mScene );
				}
				continue;
				mUniformGrid.findAll( Unit::FindEnemy( unit ), all );
				for( auto other : all ) {
					if ( other != nearest )
						mRenderingEngine->debugDrawLine( unit->getNode()->position, other->getNode()->position, ColorA(1,1,0,1 ), mScene );
				}
			}
		}
	}
	
	// Draw line from unit to move target or attack target
	if ( true ) {
		for( auto u : getUnits() ) {
			if ( u->getControlMode() == Unit::ATTACK && u->getAttackTarget() != NULL ) {
				mRenderingEngine->debugDrawLine( u->getNode()->position, u->getAttackTarget()->getNode()->position, ColorA( 1, 0, 0, 1 ), mScene );
			}
			else if ( u->getControlMode() == Unit::MOVE ) {
				mRenderingEngine->debugDrawLine( u->getNode()->position, u->getMoveTarget(), ColorA( 0, 1, 0, 1 ), mScene );
			}
		}
	}
	
	// Draw bounding boxes for selected units
	if ( true ) {
		for( auto u : getUnits() ) {
			if ( u->mIsSelected ) {
				const ColorA color = u->factionId == 0 ? ci::ColorA( 0, 1, 0, 1 ) : ci::ColorA( 1, 0, 0, 1 );
				mRenderingEngine->debugDrawCube( u->getNode()->getBoundingBox().getCenter(), u->getNode()->getBoundingBox().getSize(), color, mScene );
				
				// Draw a rectangle in screen space instead
				//const int s = 5 * RenderingEngine::get()->getContentScaleFactor();
				//const Vec2i p = u->getScreenPosition();
				//mRenderingEngine->debugScreenDrawStrokedRect( Rectf( p.x - s, p.y - s, p.x + s, p.y + s ), color );
			}
		}
	}
	
	// Draw camera position on gameplay plane
	if ( true ) {
		const float length = 100.0f;
		const float a = 0.3f;
		const ci::Vec3f p = GameCamera::get()->position;
		mRenderingEngine->debugDrawLine( p, p + Vec3f::xAxis() * length, ColorA( 1, 0, 0, a ), mScene );
		mRenderingEngine->debugDrawLine( p, p + Vec3f::yAxis() * length, ColorA( 0, 1, 0, a ), mScene );
		mRenderingEngine->debugDrawLine( p, p + Vec3f::zAxis() * length, ColorA( 0, 0, 1, a ), mScene );
	}
	
	// Draw color-coded origin vectors
	if ( true ) {const float length = 100.0f;
		mRenderingEngine->debugDrawLine( Vec3f::zero(), Vec3f::xAxis() * length, ColorA( 1, 0, 0, 1 ), mScene );
		mRenderingEngine->debugDrawLine( Vec3f::zero(), Vec3f::yAxis() * length, ColorA( 0, 1, 0, 1 ), mScene );
		mRenderingEngine->debugDrawLine( Vec3f::zero(), Vec3f::zAxis() * length, ColorA( 0, 0, 1, 1 ), mScene );
	}
}