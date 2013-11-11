#include "Scene.h"
#include "TouchInput.h"
#include "TargetConditionals.h"
#include "Game.h"

using namespace ci;

Scene* Scene::sInstance = NULL;

Scene* Scene::get()
{
	if ( sInstance == NULL ) {
		sInstance = new Scene();
	}
	return sInstance;
}

Scene::Scene() : mSkyboxNode(NULL), mBackgroundTexture(NULL)
{
	mSortTimer = ly::Timer( boost::bind( &Scene::sortSprites, this, boost::arg<1>() ), 1.0f / 60.0, 0 );
	mSortTimer.start();
}

Scene::~Scene()
{
}

void Scene::addSpriteNode( Node* node )
{
	//node->mMaterial.setTexture( "DepthMapTexture", mDepthMapFbo->mTexture );
	mSpriteNodes.push_back( node );
}

void Scene::addNode( Node* node )
{
	mObjectNodes.push_back( node );
}

void Scene::addBackgroundNode( Node* node )
{
	mBackgroundNodes.push_back( node );
}

void Scene::addCamera( GameCamera* camera )
{
	mCameras.push_back( camera );
}

void Scene::removeNode( Node* node, bool andCleanUp )
{
	{
		auto match = std::find( mObjectNodes.begin(), mObjectNodes.end(), node );
		if ( match != mObjectNodes.end() ) {
			if ( andCleanUp ) {
				delete *match;
			}
			mObjectNodes.erase( match );
			return;
		}
	}{
		auto match = std::find( mSpriteNodes.begin(), mSpriteNodes.end(), node );
		if ( match != mSpriteNodes.end() ) {
			if ( andCleanUp ) {
				delete *match;
			}
			mSpriteNodes.erase( match );
			return;
		}
	}{
		auto match = std::find( mBackgroundNodes.begin(), mBackgroundNodes.end(), node );
		if ( match != mBackgroundNodes.end() ) {
			if ( andCleanUp ) {
				delete *match;
			}
			mBackgroundNodes.erase( match );
			return;
		}
	}
}

void Scene::addNode( Node2d* node )
{
	auto match = std::find( mScreenNodes.begin(), mScreenNodes.end(), node );
	if ( match == mScreenNodes.end() ) {
		mScreenNodes.push_back( node );
	}
}

void Scene::removeNode( Node2d* node )
{
	auto match = std::find( mScreenNodes.begin(), mScreenNodes.end(), node );
	if ( match != mScreenNodes.end() ) {
		mScreenNodes.erase( match );
	}
}

void Scene::addLight( Light* light )
{
	mLights.push_back( light );
}

void Scene::removeLight( Light* light )
{
	auto match = std::find( mLights.begin(), mLights.end(), light );
	if ( match != mLights.end() ) {
		mLights.erase( match );
	}
}

void Scene::setSkyboxNode( Node* node )
{
	mSkyboxNode = node;
}

void Scene::setBackgroundTexture( Texture* texture )
{
	mBackgroundTexture = texture;
}

void Scene::sortSprites( const float deltaTime )
{
	std::sort( mSpriteNodes.begin(), mSpriteNodes.end(), Node::sortByDistanceFromCamera );
}

void Scene::update( const float deltaTime )
{
	for( auto c : mCameras ) {
		c->update( deltaTime );
		if ( mSkyboxNode ) {
			mSkyboxNode->position = c->getGlobalPosition();
			mSkyboxNode->rotation.x = 180.0f;
			mSkyboxNode->update();
		}
	}
	
	for( auto node : mObjectNodes ) {
		node->update( deltaTime );
		if ( node->mCustomDrawing != NULL ) {
			node->mCustomDrawing->update( deltaTime );
		}
	}
	
	for( auto node : mSpriteNodes ) {
		node->update( deltaTime );
		if ( node->mCustomDrawing != NULL ) {
			node->mCustomDrawing->update( deltaTime );
		}
	}
	
	for( auto node : mScreenNodes ) {
		node->update( deltaTime );
	}
	
	for( auto light : mLights ) {
		light->update( deltaTime );
	}
	
	for( auto cd : mCustomDrawings ) {
		cd->update( deltaTime );
	}
	
	mSortTimer.update( deltaTime );
	//lineTest->position += lineTest->getRight() * deltaTime * 10.0f;
}



