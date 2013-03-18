#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "Node.h"
#include "Node2d.h"
#include "GameConstants.h"
#include "Camera.h"
#include "LensFlare.h"

#include "cinder/Ray.h"

#include <list>

class Game {
public:
    Game( RenderingEngine* renderingEngine );
    ~Game();
	
    void				setup( int width, int height );
    void				update( const float deltaTime = 0.0f );
	void				debugDraw();
	
    void				touchEnded( const std::vector<ci::Vec2i>& positions );
    void				touchBegan( const std::vector<ci::Vec2i>& positions );
    void				touchMoved( const std::vector<ci::Vec2i>& positions );
	
	Node2d*				getRootGui() const { return mRootGui; }
	Camera*				getCamera() const { return mCamera; }
	ResourceManager*	getResourceManager() const { return mResourceManager; }
	RenderingEngine*	getRenderingEngine() const { return mRenderingEngine; }
	
	void				add( Node* node );
	void				remove( Node* node );
	void				remove( Node2d* Node2d );
	
	bool				rayCast( const ci::Ray& ray );
	
private:
	Camera* mCamera;
	
	Node2d* mRootGui;
	Node* mClouds;
	Node* mPlanet;
	std::list<Node*> mNodes;
	
    RenderingEngine* mRenderingEngine;
    ResourceManager* mResourceManager;
    
	float mTouchDistanceStart;
	float mTouchDistanceCurrent;
	float mZoomStart;
	
	ci::Vec2i mTouchStart;
	ci::Vec2i mTouchCurrent;
	ci::Vec3f mStartRotation;
	
	LensFlare* mLensFlare;
};