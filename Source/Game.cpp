#include "Game.h"
#include "Touch.h"

using namespace ci;

Game::Game(RenderingEngine* renderingEngine ) : mRenderingEngine(renderingEngine)
{
}

Game::~Game()
{
    delete mRenderingEngine;
}

void Game::add( Node* node )
{
	mRenderingEngine->addNode( node );
	mNodes.push_back( node );
}

void Game::add( Gui* gui )
{
	mRenderingEngine->addGuiNode( gui );
	mRootGui->addChild( gui );
}

void Game::remove( Node* node ) {}

void Game::remove( Gui* gui ) {}

void Game::setup( int width, int height )
{
    mScreenSize = ci::Vec2i( width, height );
	
	mCamera = Camera::get();
	
	mResourceManager = new ResourceManager( mRenderingEngine );
	
	mResourceManager->loadShader( kShaderFragmentLighting,	"shaders/pixel_lighting.vert",		"shaders/pixel_lighting_plus.frag" );
	mResourceManager->loadShader( kSahderVertexLighting,	"shaders/vertex_lighting.vert",		"shaders/vertex_lighting.frag" );
	mResourceManager->loadShader( kShaderUnlit,				"shaders/unlit.vert",				"shaders/unlit.frag" );
	mResourceManager->loadShader( kShaderGui2d,				"shaders/gui.vert",					"shaders/gui.frag" );
	
	mResourceManager->loadTexture( "textures/mars_diffuse.png" );
	mResourceManager->loadTexture( "textures/mars_normal.jpg" );
	mResourceManager->loadTexture( "textures/stars.jpg" );
	mResourceManager->loadTexture( "textures/sphere_glow.png" );
	mResourceManager->loadTexture( "textures/sun_sprite.png" );
	mResourceManager->loadTexture( "textures/metal.png" );
	
	mResourceManager->loadMesh( "models/sphere_globe.obj" );
	mResourceManager->loadMesh( "models/skybox.obj" );
	mResourceManager->loadMesh( "models/tower.obj" );
	mResourceManager->loadMesh( "models/quad_plane.obj" );
	
	// Create VBOs for our geometry
	mPlanet					= new Node();
	mPlanet->mMesh			= mResourceManager->getMesh( "models/sphere_globe.obj" );
    mPlanet->mColorSpecular	= ci::Vec4f( .5, .5, .5, 1.0 );
    mPlanet->mColorRim		= ci::Vec4f( 0.3, 0.15, 0.0, 0.5 );
	mPlanet->mRimPower		= 0.2f;
	mPlanet->mShininess		= 50.0f;
	mPlanet->mShader		= kShaderFragmentLighting;
	mPlanet->mTexture		= mResourceManager->getTexture( "textures/mars_diffuse.png" );
	mPlanet->mTextureNormal = mResourceManager->getTexture( "textures/mars_normal.jpg" );
	mPlanet->scale			= Vec3f::one() * 0.6f;
	add( mPlanet );
	
	Node* skyBox			= new Node();
	skyBox->mMesh			= mResourceManager->getMesh( "models/skybox.obj" );
	skyBox->mTexture		= mResourceManager->getTexture( "textures/stars.jpg" );
	skyBox->mShader			= kShaderUnlit;
	skyBox->scale			= Vec3f::one() * 100.0f;
	add( skyBox );
	
	for( int i = 0; i < 20; i++ ) {
		Node* tower				= new Node();
		tower->mMesh			= mResourceManager->getMesh( "models/tower.obj" );
		tower->mColorSpecular	= ci::Vec4f( 0.5, 0.5, 0.5, 1.0 );
		tower->mShininess		= 100.0f;
		tower->mShader			= kSahderVertexLighting;
		tower->rotation.x		= arc4random() % 360;
		tower->rotation.y		= arc4random() % 360;
		tower->rotation.z		= arc4random() % 360;
		tower->scale			= Vec3f::one() * 0.05f;
		tower->pivotOffset.y	= 1.0f;
		tower->mTexture			= mResourceManager->getTexture( "textures/metal.png" );
		tower->setParent( mPlanet );
		mRenderingEngine->addNode( tower );
		mNodes.push_back( tower );
	}
	
	Node* glowSprite		= new Node();
	glowSprite->mFaceCamera = true;
	glowSprite->mMesh		= mResourceManager->getMesh( "models/quad_plane.obj" );
	glowSprite->mTexture	= mResourceManager->getTexture( "textures/sphere_glow.png" );
    glowSprite->mColor		= ci::Vec4f( 0.6, 0.3, 0.0, 0.5 );
	glowSprite->mShader		= kShaderUnlit;
	glowSprite->scale		= Vec3f::one() * 1.51f;
	add( glowSprite );
	
	mCamera->setZoom( 4.0f );
	mZoomStart = mCamera->getZoom();
	mTouchDistanceCurrent = 0.0f;
	
	mRootGui = new Gui();
	mRootGui->getNode()->mMesh = mResourceManager->getMesh( "models/quad_plane.obj" );;
	/*mRootGui->setTexture( texture );
	mRootGui->position = Vec2i( 400, 400 );
	mRootGui->size = Vec2i( 200, 100 );
	mRootGui->anchor = Vec2f( 0.5f, 0.5f );*/
	mRenderingEngine->addGuiNode( mRootGui );
	
	Gui* sunSprite = new Gui();
	sunSprite->getNode()->mMesh = mResourceManager->getMesh( "models/quad_plane.obj" );
	sunSprite->setTexture( mResourceManager->getTexture( "textures/sun_sprite.png" ) );
	sunSprite->getNode()->mShader = kShaderGui2d;
	sunSprite->size = Vec2i( 300, 300 );
	sunSprite->anchor = Vec2f( 0.5f, 0.5f );
	add( sunSprite );
	
	/*Gui* child = new Gui();
	child->getNode()->mMesh = planeMesh;
	child->setTexture( texture );
	child->position = Vec2i( 0, 300 );
	child->size = Vec2i( 50, 50 );
	child->anchor = Vec2f( 0.5f, 0.5f );
	mRenderingEngine->addGuiNode( child );
	add( child );*/
}

void Game::update( const float deltaTime )
{
	mCamera->update( deltaTime );
	
	float targetY = mStartRotation.y + ( mTouchCurrent.x - mTouchStart.x ) * -0.3f;
	float targetX = mStartRotation.x + ( mTouchCurrent.y - mTouchStart.y ) * -0.3f;
	const float maxAngle = 89.9f;
	const float minAngle = -89.9f;
	targetX = math<float>::clamp( targetX, minAngle, maxAngle );
	mCamera->setAngle( targetX );
	mCamera->rotation.y = targetY;
	float targetZ = mZoomStart + ( mTouchDistanceCurrent - mTouchDistanceStart ) * -0.01f;
	const float maxZoom = 3.0f;
	const float minZoom = 0.65f;
	targetZ = math<float>::clamp( targetZ, minZoom, maxZoom );
	mCamera->setZoom( targetZ );
	
	const float planetRotationSpeed = 4.0f * deltaTime;
	mPlanet->rotation.y += planetRotationSpeed;
	//mCamera->rotation.y += planetRotationSpeed * (mCamera->getZoom() - minZoom) / (maxZoom - minZoom);
	
	mRootGui->update( deltaTime );
	
	for( auto iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		node->update( deltaTime );
	}
}

void Game::touchEnded( const std::vector<ci::Vec2i>& positions )
{
}

void Game::touchBegan( const std::vector<ci::Vec2i>& positions )
{
	if ( positions.size() == 1 ) {
		mStartRotation = Vec3f( mCamera->getAngle(), mCamera->rotation.y, 0.0f );
		mTouchStart = mTouchCurrent = positions[0];
	}
	else if ( positions.size() >=2  ) {
		mTouchDistanceStart = mTouchDistanceCurrent = Touch::getDistance( positions );
		mZoomStart = mCamera->getZoom();
	}
}

void Game::touchMoved( const std::vector<ci::Vec2i>& positions )
{
	if ( positions.size() == 1 ) {
		mTouchCurrent = positions[0];
	}
	else if ( positions.size() >=2  ) {
		mTouchDistanceCurrent = Touch::getDistance( positions );
	}
}



