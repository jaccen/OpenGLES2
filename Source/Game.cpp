#include "Game.h"
#include "Touch.h"

using namespace ci;

Game::Game(RenderingEngine* renderingEngine ) : mRenderingEngine(renderingEngine), mFreeTargetMode( true ), mPlanet(NULL), mLensFlare(NULL), mAngleTargetX( 0.0f ), mAngleTargetY( 0.0f )
{
}

Game::~Game()
{
    delete mRenderingEngine;
}

void Game::remove( Node2d* Node2d ) {}

void Game::setup( int width, int height )
{	
	mTouch.addDelegate( this );
	
	mResourceManager = ResourceManager::get();
	mResourceManager->setup( mRenderingEngine );
	
	mResourceManager->loadShader( kShaderText,				"shaders/text.vert",				"shaders/text.frag" );
	mResourceManager->loadShader( kShaderFragmentLighting,	"shaders/planet.vert",				"shaders/planet.frag" );
	mResourceManager->loadShader( kSahderClouds,			"shaders/pixel_lighting.vert",		"shaders/clouds.frag" );
	mResourceManager->loadShader( kSahderVertexLighting,	"shaders/vertex_lighting.vert",		"shaders/vertex_lighting.frag" );
	mResourceManager->loadShader( kShaderUnlit,				"shaders/unlit.vert",				"shaders/unlit.frag" );
	mResourceManager->loadShader( kShaderScreenSpace,		"shaders/screen_space.vert",		"shaders/screen_space.frag" );
	mResourceManager->loadShader( kShaderScreenSpaceBW,		"shaders/screen_space.vert",		"shaders/black_and_white.frag" );
	mResourceManager->loadShader( kShaderDebug,				"shaders/debug.vert",				"shaders/debug.frag" );
	mResourceManager->loadShader( kShaderShip,				"shaders/ship.vert",				"shaders/ship.frag" );
	mResourceManager->loadShader( kShaderScreenText,		"shaders/screen_space.vert",		"shaders/screen_space_text.frag" );
	
	mResourceManager->loadMesh( "models/sphere_globe.obj" );
	mResourceManager->loadMesh( "models/skybox.obj" );
	//mResourceManager->loadMesh( "models/tower.obj" );
	mResourceManager->loadMesh( "models/quad_plane.obj" );
	
	mCamera = Camera::get();
	
	mPlanet					= new Node();
	mPlanet->mLayer			= Node::LayerObjects;
	mPlanet->mMesh			= mResourceManager->getMesh( "models/sphere_globe.obj" );
	mPlanet->mColorSelfIllumination	= ci::Vec4f( 1, 0, 0, 1.0 );
	mPlanet->mColorRim		= ci::Vec4f( 1, .15, 0, 0.1 );
    mPlanet->mColorSpecular	= ci::Vec4f( 1, 1, 1, 0.8f );
	mPlanet->mRimPower		= 0.15f;
	mPlanet->mShininess		= 50.0f;
	mPlanet->mGlossiness	= 0.0f;
	mPlanet->mShader		= kSahderVertexLighting;
	mPlanet->mTexture		= mResourceManager->getTexture( "textures/mars_diffuse.png" );
	mPlanet->mTextureNormal = mResourceManager->getTexture( "textures/mars_normal.jpg" );
	mPlanet->mTextureSelfIllumination = mResourceManager->getTexture( "textures/light_map.jpg" );
	mPlanet->scale			= Vec3f::one() * 3.6f;
	mRenderingEngine->addNode( mPlanet );
	
	Node* glowSprite		= new Node();
	glowSprite->mLayer		= Node::LayerLighting;
	glowSprite->mFaceCamera = true;
	glowSprite->mMesh		= mResourceManager->getMesh( "models/quad_plane.obj" );
	glowSprite->mTexture	= mResourceManager->getTexture( "textures/sphere_glow.png" );
    glowSprite->mColor		= ci::Vec4f( 1, .5, 0, 0.4 );
	glowSprite->mShader		= kShaderUnlit;
	glowSprite->scale		= Vec3f::one() * 9.6f;
	mRenderingEngine->addNode( glowSprite );
	
	mFocusTarget = mPlanet;
	
	Node* skyBox			= new Node();
	skyBox->mMesh			= mResourceManager->getMesh( "models/skybox.obj" );
	skyBox->mTexture		= mResourceManager->getTexture( "textures/stars2.jpg" );
	skyBox->mShader			= kShaderUnlit;
	skyBox->scale			= Vec3f::one() * 400.0f;
	mRenderingEngine->setSkyboxNode( skyBox );
	
	
	for( int i = 0; i < 6; i++ ) {
		Node* glowSprite		= new Node();
		glowSprite->mLayer		= Node::LayerLighting;
		glowSprite->mFaceCamera = true;
		glowSprite->mMesh		= mResourceManager->getMesh( "models/quad_plane.obj" );
		glowSprite->mTexture	= mResourceManager->getTexture( "textures/projectile.png" );
		glowSprite->mColor		= ci::Vec4f( 1, .5, 0, 1.0f );
		glowSprite->mShader		= kShaderUnlit;
		glowSprite->scale		= Vec3f(0.5,0.5,0.5) * 1.0f;
		int rX = arc4random() % 100;
		int rY = arc4random() % 100;
		int rZ = arc4random() % 100;
		glowSprite->position	= Vec3f( rX, rY, rZ ) * 0.05f;
		mRenderingEngine->addNode( glowSprite );
	}
	
	for( int i = 0; i < 6; i++ ) {
		int rX = arc4random() % 360;
		int rY = arc4random() % 360;
		int rZ = arc4random() % 360;
		Node* tower				= new Node();
		tower->mMesh			= mResourceManager->getMesh( "models/cube.obj" );
		tower->mColorSpecular	= ci::Vec4f( 1, 1, 1, 1.0 );
		tower->mShininess		= 100.0f;
		tower->mShader			= kSahderVertexLighting;
		tower->rotation			= Vec3f( rX, rY, rZ );
		tower->scale			= Vec3f( 1, 2, 1 ) * 0.04f;
		tower->pivotOffset.y	= 0.98f;
		tower->mTexture			= mResourceManager->getTexture( "textures/metal.png" );
		tower->mTextureSelfIllumination = mResourceManager->getTexture( "textures/ship_light_map.png" );
		tower->mColorSelfIllumination = Vec4f( 1, 0, 0, 1 );
		tower->setParent( mPlanet );
		mRenderingEngine->addNode( tower );
		mNodes.push_back( tower );
		mControllers.push_back( new ObjectController( tower ) );
		tower->update();
	}
	
	for( int i = 0; i < 30; i++ ) {
		Node* shipNode				= new Node();
		shipNode->mLayer			= Node::LayerObjects;
		shipNode->mMesh			= mResourceManager->getMesh( "models/cube.obj" );
		shipNode->mColorSpecular	= ci::Vec4f( 1, 1, 1, 1.0 );
		shipNode->mShininess		= 100.0f;
		shipNode->mGlossiness		= 1.0f;
		shipNode->mShader			= kSahderVertexLighting;
		shipNode->scale			= Vec3f( 1, 2, 3 ) * 0.05f;
		int rX = arc4random() % 200 - 100;
		int rY = arc4random() % 200 - 100;
		int rZ = arc4random() % 200 - 100;
		shipNode->position			= Vec3f( rX, rY, rZ ) * 0.15f;
		shipNode->mTexture			= mResourceManager->getTexture( "textures/metal.png" );
		shipNode->mTextureSelfIllumination = mResourceManager->getTexture( "textures/ship_light_map.png" );
		shipNode->mColorSelfIllumination = Vec4f( .5, .5, 1, 1 );
		mRenderingEngine->addNode( shipNode );
		ObjectController* ship = new ObjectController( shipNode, arc4random() % 2 );
		mControllers.push_back( ship);
	}
	
	mCamera->setZoom( 50.0f );
	mZoomStart = mZoomTarget = mCamera->getZoom();
	mCamera->setAngle( -10.0f );
	mStartRotation = mRotationTarget = Vec3f( mCamera->getAngle(), mCamera->rotation.y, 0.0f );
	
	mLensFlare = new LensFlare( this );
	
	mRootGui = new Node2d();
	mRenderingEngine->addNode( mRootGui );
	
	Font* font = ResourceManager::get()->getFont( "fonts/menlo.fnt" );
	
	Node2d* child = new Node2d();
	child->mText = new Text( font );
	child->mText->scale = 0.4f;
	child->mText->padding = 25;
	child->mText->width = 256;
	child->mText->height = 150;
	child->mText->letterSpacing = 2;
	child->mText->mColor = Vec4f( 1, 1, 1, 1 );
	child->mText->truncateWithDots = true;
	child->mText->setText( "\"The 14210 quick brown foxes jumped over the 21543 lazy dogs,\" I said.  \"The 1440 quick brown foxes jumped over the 5432142 lazy dogs,\" I said.  \"The 10 quick brown foxes jumped over the 52 lazy dogs,\" I said.  \"The 4320 quick brown foxes jumped over the 52 lazy dogs,\" I said." );
	child->getNode()->mLayer = Node::LayerGui;
	Texture* t = mResourceManager->getTexture( "textures/test_dialog.png" );
	child->setTexture( t );
	child->position = Vec2i( 0, 0 );
	child->anchor = Vec2f( 0.0f, 0.0f );
	mRootGui->addChild( child );
}

void Game::update( const float deltaTime )
{
	mCamera->update( deltaTime );
	
	mTouch.update( deltaTime );
	
	mCamera->position += ( mFocusTarget->getGlobalPosition() - mCamera->position ) / 20.0f;
	
	// Zooming
	float targetZ = mZoomStart + mTouch.getTouchesDistance() * -(0.15f);
	const float maxZoom = 100.0f;
	const float minZoom = 9.65f;
	targetZ = math<float>::clamp( targetZ, minZoom, maxZoom );
	mZoomTarget += (targetZ - mZoomTarget ) / 10.0f;
	mCamera->setZoom( mZoomTarget );
	
	// Orbiting
	float targetY = mStartRotation.y + mTouch.getTouchesDifference().x * -0.3f;
	float targetX = mStartRotation.x + mTouch.getTouchesDifference().y * -0.3f;
	const float maxAngle = 89.9f;
	const float minAngle = -89.9f;
	targetX = math<float>::clamp( targetX, minAngle, maxAngle );
	float easing = 10.0f;
	mAngleTargetX += ( targetX - mAngleTargetX ) / easing;
	mCamera->setAngle( mAngleTargetX );
	mAngleTargetY += ( targetY - mAngleTargetY ) / easing;
	mCamera->rotation.y = mAngleTargetY;
	
	if ( mPlanet ) {
		const float planetRotationSpeed = 4.0f;
		mPlanet->rotation.y += planetRotationSpeed * deltaTime;
	}
	
	if ( mLensFlare ) {
		mLensFlare->update( deltaTime );
	}
	
	for( auto iter = mControllers.begin(); iter != mControllers.end(); iter++ ) {
		(*iter)->update( deltaTime );
	}
}

void Game::tapDown(ci::Vec2i position)
{
	if ( Node* node = pickObject( mCamera->rayIntoScene( position ) ) ) {
		//std::cout << "tapDown: " << position << std::endl;
		//mFocusTarget = node;
		//mFreeTargetMode = false;
	}
}

void Game::debugDraw()
{
	return;
	
	// Draw grid
	Vec4f c = Vec4f( 1, 1, 1, 0.1f );
	float l = 1.5f;
	int n = 20;
	for( int i = -n; i <= n; i++ ) {
		mRenderingEngine->debugDrawLine( Vec3f(i,0,-n) * l, Vec3f(i,0,n) * l, c );
		mRenderingEngine->debugDrawLine( Vec3f(-n,0,i) * l, Vec3f(n,0,i) * l, c );
	}
	
	// Draw bounding boxes
	for( auto iter = mControllers.begin(); iter != mControllers.end(); iter++ ) {
		ObjectController* controller = *iter;
		const AxisAlignedBox3f& b = controller->getBoundingBox();
		Vec4f color = controller->getNode() == mFocusTarget ? Vec4f( 0, 1, 0, 1 ) : Vec4f( 1, 1, 1, 0.4f );
		mRenderingEngine->debugDrawCube( b.getCenter(), b.getSize(), color );
	}
}

Node* Game::pickObject( const ci::Ray& ray )
{
	for( auto iter = mControllers.begin(); iter != mControllers.end(); iter++ ) {
		ObjectController* controller = *iter;
		Node* node = controller->getNode();
		
		if ( node->mLayer != Node::LayerObjects ) continue;
		if ( node->mMesh == NULL ) continue;
		if ( node == mFocusTarget ) continue;
		
		if( controller->getBoundingBox().intersects( ray ) ) {
			return node;
		}
	}
	return NULL;
}

void Game::gestureStarted( int fingerCount )
{
	if ( fingerCount == 2 ) {
		mZoomStart = mCamera->getZoom();
		mStartRotation = Vec3f( mCamera->getAngle(), mCamera->rotation.y, 0.0f );
	}
}

void Game::gestureEnded( int fingerCount )
{
	if ( fingerCount == 2 ) {
	}
}


