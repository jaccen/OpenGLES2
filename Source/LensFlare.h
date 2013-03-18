#pragma once

#include "cinder/Vector.h"

#include <list>

#include "Node2d.h"

class Game;

class LensFlare {
public:
	struct Sprite {
		int size;
		ci::Vec4f color;
		std::string texture;
	};
	
	LensFlare( Game* game );
	virtual ~LensFlare();
	void update( const float deltaTime = 0.0f );
	
private:
	Node2d mRoot;
	Game* mGame;
	ci::Vec3f mLightPosition;
	std::list<Node2d*> mSprites;
};