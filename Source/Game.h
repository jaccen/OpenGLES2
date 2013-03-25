#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "Node.h"
#include "Node2d.h"
#include "GameConstants.h"
#include "Camera.h"
#include "LensFlare.h"
#include "Touch.h"

#include "ObjectController.h"

#include "cinder/Ray.h"

#include <list>

class Game : Touch::IDelegate {
public:
    Game( RenderingEngine* renderingEngine );
    ~Game();
	
    void				setup( int width, int height );
    void				update( const float deltaTime = 0.0f );
	void				debugDraw();
	
    void				touchesBegan( const std::vector<ci::Vec2i>& positions ) { mTouch.touchesBegan( positions ); }
    void				touchesMoved( const std::vector<ci::Vec2i>& positions ) { mTouch.touchesMoved( positions ); }
    void				touchesEnded( const std::vector<ci::Vec2i>& positions ) { mTouch.touchesEnded( positions ); }
	
	Node2d*				getRootGui() const { return mRootGui; }
	Camera*				getCamera() const { return mCamera; }
	ResourceManager*	getResourceManager() const { return mResourceManager; }
	
	void				remove( Node* node );
	void				remove( Node2d* Node2d );
	
	Node*				pickObject( const ci::Ray& ray );
	
	const std::list<Node*>& getNodes() { return mNodes; }
	
	void				gestureStarted( int fingerCount );
	void				gestureEnded( int fingerCount );
	void				tapDown(ci::Vec2i position);
	void				tapUp(ci::Vec2i position) {}
	
private:
	Camera* mCamera;
	std::list<ObjectController*> mControllers;
	
	Touch mTouch;
	
	Node2d* mRootGui;
	Node* mPlanet;
	std::list<Node*> mNodes;
	
    RenderingEngine* mRenderingEngine;
    ResourceManager* mResourceManager;
	
	// Camera controls
	bool mFreeTargetMode;
	Node* mFocusTarget;
	float mZoomStart;
	float mZoomTarget;
	ci::Vec3f mStartRotation;
	ci::Vec3f mRotationTarget;
	
	LensFlare* mLensFlare;
};