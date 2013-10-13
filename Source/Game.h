#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "Node.h"
#include "Node2d.h"
#include "GameConstants.h"
#include "Camera.h"
#include "LensFlare.h"
#include "TouchInput.h"
#include "Text.h"
#include "CameraController.h"

#include "ObjectController.h"

#include "cinder/Ray.h"

#include <list>

class Game {
public:
    Game( RenderingEngine* renderingEngine );
    ~Game();
	
    void				setup( int width, int height );
    void				update( const float deltaTime = 0.0f );
	void				debugDraw();
	
	Node2d*				getRootGui() const { return mRootGui; }
	Camera*				getCamera() const { return mCamera; }
	ResourceManager*	getResourceManager() const { return mResourceManager; }
	
	void				remove( Node* node );
	void				remove( Node2d* Node2d );
	
	ObjectController*	pickObject( const ci::Ray& ray );
	
	const std::list<Node*>& getNodes() { return mNodes; }
	
private:
	Camera* mCamera;
	EditorCamera mCameraController;
	
	Node2d* mRootGui;
	Node* mPlanet;
	std::list<Node*> mNodes;
	
    RenderingEngine* mRenderingEngine;
    ResourceManager* mResourceManager;
		
	void selectController( ObjectController* controller );
	void deselectController( ObjectController* controller );
	void deselectAllControllers();
};