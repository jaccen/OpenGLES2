#include "Game.h"
#include "TouchInput.h"
#include "TargetConditionals.h"
#include "ProjectileManager.h"
#include "Planet.h"

using namespace ci;

Game* Game::sInstance = NULL;

Game* Game::get()
{
	if ( sInstance == NULL ) {
		sInstance = new Game();
	}
	return sInstance;
}

Game::Game()
{
	mRenderer = RenderingEngine::get();
}

Game::~Game()
{
}

void Game::setup( const int width, const int height, const float contentScaleFactor )
{
	mScreenSize = Vec2i( width, height );
	mContentScaleFactor = contentScaleFactor;
	
	mRenderer->setup( width, height, contentScaleFactor );
	ResourceManager::get()->setup( mRenderer );
}

void Game::update( const float deltaTime )
{
	for( auto scene : mScenes ) {
		scene->update( deltaTime );
	}
	
	for( auto c : mControllers ) {
		c->update( deltaTime );
	}
}

void Game::draw()
{
	mRenderer->draw( mScenes );
}

void Game::addScene( Scene* node )
{
	auto match = std::find( mScenes.begin(), mScenes.end(), node );
	if ( match == mScenes.end() ) {
		mScenes.push_back( node );
	}
}

void Game::removeScene( Scene* node )
{
	auto match = std::find( mScenes.begin(), mScenes.end(), node );
	if ( match != mScenes.end() ) {
		mScenes.erase( match );
	}
}

void Game::addController( Controller* controller )
{
	auto match = std::find( mControllers.begin(), mControllers.end(), controller );
	if ( match == mControllers.end() ) {
		mControllers.push_back( controller );
	}
}

void Game::removeController( Controller* controller )
{
	auto match = std::find( mControllers.begin(), mControllers.end(), controller );
	if ( match != mControllers.end() ) {
		mControllers.erase( match );
	}
}