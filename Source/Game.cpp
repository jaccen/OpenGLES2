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

void Game::remove( Node2d* Node2d ) {}

void Game::setup( int width, int height )
{
	mCamera = Camera::get();
	
	mResourceManager = ResourceManager::get();
	mResourceManager->setup( mRenderingEngine );
	
	mResourceManager->loadShader( kShaderFragmentLighting,	"shaders/planet.vert",				"shaders/planet.frag" );
	mResourceManager->loadShader( kSahderClouds,			"shaders/pixel_lighting.vert",		"shaders/clouds.frag" );
	mResourceManager->loadShader( kSahderVertexLighting,	"shaders/vertex_lighting.vert",		"shaders/vertex_lighting.frag" );
	mResourceManager->loadShader( kShaderUnlit,				"shaders/unlit.vert",				"shaders/unlit.frag" );
	mResourceManager->loadShader( kShaderScreenSpace,		"shaders/screen_space.vert",		"shaders/screen_space.frag" );
	mResourceManager->loadShader( kShaderDebug,				"shaders/debug.vert",				"shaders/debug.frag" );
	
	mResourceManager->loadMesh( "models/sphere_globe.obj" );
	mResourceManager->loadMesh( "models/skybox.obj" );
	mResourceManager->loadMesh( "models/tower.obj" );
	mResourceManager->loadMesh( "models/quad_plane.obj" );
	
	// Create VBOs for our geometry
	mPlanet					= new Node();
	mPlanet->mLayer			= Node::LayerObjects;
	mPlanet->mMesh			= mResourceManager->getMesh( "models/sphere_globe.obj" );
    mPlanet->mColorSelfIllumination	= ci::Vec4f( 1, 0, 0, 1.0 );
    mPlanet->mColorRim		= ci::Vec4f( 1, .15, 0, 0.1 );
    mPlanet->mColorSpecular	= ci::Vec4f( 1, 1, 1, 0.8f );
	mPlanet->mRimPower		= 0.15f;
	mPlanet->mShininess		= 50.0f;
	mPlanet->mGlossiness	= 1.0f;
	mPlanet->mShader		= kShaderFragmentLighting;
	mPlanet->mTexture		= mResourceManager->getTexture( "textures/mars_diffuse.png" );
	mPlanet->mTextureNormal = mResourceManager->getTexture( "textures/mars_normal.jpg" );
	mPlanet->mTextureSelfIllumination = mResourceManager->getTexture( "textures/light_map.jpg" );
	mPlanet->scale			= Vec3f::one() * 0.6f;
	add( mPlanet );
	
	Node* skyBox			= new Node();
	skyBox->mMesh			= mResourceManager->getMesh( "models/skybox.obj" );
	skyBox->mTexture		= mResourceManager->getTexture( "textures/stars2.jpg" );
	skyBox->mShader			= kShaderUnlit;
	skyBox->scale			= Vec3f::one() * 100.0f;
	//mRenderingEngine->setSkyboxNode( skyBox );
	
	for( int i = 0; i < 6; i++ ) {
		int rX = arc4random() % 360;
		int rY = arc4random() % 360;
		int rZ = arc4random() % 360;
		for( int k = 0; k < 20; k++ ) {
			Node* tower				= new Node();
			tower->mMesh			= mResourceManager->getMesh( "models/cube.obj" );
			tower->mColorSpecular	= ci::Vec4f( 1, 1, 1, 1.0 );
			tower->mShininess		= 100.0f;
			tower->mShader			= kSahderVertexLighting;
			int s = 3;
			tower->rotation.x		= rX + arc4random() % s;
			tower->rotation.y		= rY + arc4random() % s;
			tower->rotation.z		= rZ + arc4random() % s;
			int height = 2 + arc4random() % 30;
			tower->scale			= Vec3f( 0.15, (float) height / 20.0, 0.15 ) * 0.08f;
			tower->pivotOffset.y	= 0.98f;
			tower->mTexture			= mResourceManager->getTexture( "textures/metal.png" );
			tower->setParent( mPlanet );
			mRenderingEngine->addNode( tower );
			mNodes.push_back( tower );
			tower->update();
			if ( k == 1 ) {
				mCities.push_back( tower );
			}
		}
	}
	
	Node* glowSprite		= new Node();
	glowSprite->mLayer		= Node::LayerLighting;
	glowSprite->mFaceCamera = true;
	glowSprite->mMesh		= mResourceManager->getMesh( "models/quad_plane.obj" );
	glowSprite->mTexture	= mResourceManager->getTexture( "textures/sphere_glow.png" );
    glowSprite->mColor		= ci::Vec4f( 1, 1, 1, 0.4 );
	glowSprite->mShader		= kShaderUnlit;
	glowSprite->scale		= Vec3f::one() * 1.54f;
	add( glowSprite );
	
	// Create VBOs for our geometry
	mClouds					= new Node();
	mClouds->mLayer			= Node::LayerClouds;
	mClouds->mMesh			= mResourceManager->getMesh( "models/sphere_globe.obj" );
	mClouds->mShader		= kSahderClouds;
	mClouds->mTexture		= mResourceManager->getTexture( "textures/clouds.png" );
	mClouds->mTextureNormal = mResourceManager->getTexture( "textures/clouds_normal.jpg" );
	mClouds->scale			= Vec3f::one() * 0.61f;
	//add( mClouds );
	
	mCamera->setZoom( 4.0f );
	mZoomStart = mCamera->getZoom();
	mTouchDistanceCurrent = 0.0f;
	
	mLensFlare = new LensFlare( this );
	
	mRootGui = new Node2d();
	mRenderingEngine->addNode( mRootGui );
	
	Texture* texture =  mResourceManager->getTexture( "textures/city_icon.png" );
	for( int i = 0; i < mCities.size(); i++ ) {
		Node2d* child = new Node2d();
		child->getNode()->mLayer = Node::LayerGui;
		child->setTexture( texture );
		child->position = Vec2i( 100, 100 );
		child->setColor( Vec4f( 0, 1, 0, 1 ) );
		child->size = Vec2i( texture->mWidth, texture->mHeight );
		child->anchor = Vec2f( 0.5f, -.2f );
		mRootGui->addChild( child );
		mCityIcons.push_back( child );
	}
	
	Node2d* child = new Node2d();
	child->getNode()->mLayer = Node::LayerGui;
	child->setTexture( mResourceManager->getTexture( "textures/metal.png" ) );
	child->position = Vec2i( 0, 0 );
	child->size = Vec2i( 768, 100 );
	child->anchor = Vec2f( 0.0f, 0.0f );
	mRootGui->addChild( child );
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
	const float maxZoom = 8.0f;
	const float minZoom = 0.65f;
	targetZ = math<float>::clamp( targetZ, minZoom, maxZoom );
	mCamera->setZoom( targetZ );
	
	const float planetRotationSpeed = 4.0f;
	mPlanet->rotation.y += planetRotationSpeed * deltaTime;
	mClouds->rotation.y += ( planetRotationSpeed + 0.5f ) * deltaTime;
	//mCamera->rotation.y += planetRotationSpeed * (mCamera->getZoom() - minZoom) / (maxZoom - minZoom);
	
	mRootGui->update( deltaTime );
	
	for( int i = 0; i < mCities.size(); i++ ) {
		Node2d* node = mCityIcons[i];
		node->position = mCamera->getWorldToScreen( mCities[i]->getGlobalPosition() );
		node->update();
	}
	
	for( auto iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		node->update( deltaTime );
	}
	
	mLensFlare->update( deltaTime );
}

void Game::debugDraw()
{
	mLensFlare->debugDraw();
	
	// Draw cubic bounding boxes
	/*for( auto iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		if ( node->mMesh == NULL ) continue;
		
		AxisAlignedBox3f worldBounds = node->mMesh->triMesh.calcBoundingBox( node->getTransform() );
		mRenderingEngine->debugDrawCube( worldBounds.getCenter(), worldBounds.getSize(), Vec4f( 1, 0, 0, 1 ) );
	}*/
}

bool Game::rayCast( const ci::Ray& ray )
{
	for( auto iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		if ( node->mLayer == Node::LayerLighting || node->mLayer == Node::LayerNone ) continue;
		if ( node->mMesh == NULL ) continue;
		
		// TODO: Get this fracking optimization working:
		//AxisAlignedBox3f worldBounds = node->mMesh->triMesh.calcBoundingBox( node->getTransform() );
		//if( worldBounds.intersects( ray ) ) {
			float distance = 0.0f;
			int len = node->mMesh->triMesh.getNumTriangles();
			for( size_t i = 0; i < len; i++ ) {
				Vec3f v0, v1, v2;
				node->mMesh->triMesh.getTriangleVertices(i, &v0, &v1, &v2);
				v0 = node->getTransform().transformPointAffine(v0);
				v1 = node->getTransform().transformPointAffine(v1);
				v2 = node->getTransform().transformPointAffine(v2);
				if( ray.calcTriangleIntersection(v0, v1, v2, &distance) ) {
					return true;
				}
			}
		//}
	}
	return false;
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



