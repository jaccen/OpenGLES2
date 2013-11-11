#pragma once

#include "ObjParser.h"
#include "ResourceManager.h"
#include "RenderingEngine.h"
#include "ProjectileManager.h"
#include "Node.h"
#include "Node2d.h"
#include "GameConstants.h"
#include "GameCamera.h"
#include "LensFlare.h"
#include "TouchInput.h"
#include "Text.h"
#include "Controls.h"
#include "Unit.h"
#include "Light.h"
#include "UniformGrid.h"

#include "cinder/Ray.h"

#include <list>

class Scene {
public:
	static Scene*			get();
	
    void					update( const float deltaTime);
	
	void					setRootGui( Node2d* Node2d );
	void					addNode( Node* node );
	void					removeNode( Node* node, bool andCleanUp = true );
	void					addNode( Node2d* node );
	void					addBackgroundNode( Node* node );
	void					removeNode( Node2d* node );
	void					addSpriteNode( Node* node );
	void					addLight( Light* light );
	void					removeLight( Light* light );
	void					setBackgroundTexture( Texture* texture );
	void					setSkyboxNode( Node* node );
	void					addCamera( GameCamera* camera );
	
	GameCamera*				getCamera( const std::string name ) { return mCameras[ 0 ]; }
	GameCamera*				getCamera( const int index = 0 ) { return mCameras[ index ]; }
	const std::list<Node*>& getObjectNodes() const { return mObjectNodes; }
	
	std::list<Node*>		mObjectNodes;
	std::list<Node*>		mBackgroundNodes;
	std::list<Node2d*>		mScreenNodes;
	std::vector<Node*>		mSpriteNodes;
	std::list<Light*>		mLights;
	Node*					mSkyboxNode;
	Texture*				mBackgroundTexture;
	inline void				sortSprites( const float deltaTime );
	std::list<CustomDrawing*> mCustomDrawings;
	
private:
    Scene();
    ~Scene();
	
	static Scene*			sInstance;
	ly::Timer				mSortTimer;
	std::vector<GameCamera*> mCameras;
};