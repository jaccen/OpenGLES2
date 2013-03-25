#pragma once

#include "cinder/Vector.h"
#include "cinder/Ray.h"

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
	void debugDraw();
	virtual ~LensFlare();
	void update( const float deltaTime = 0.0f );
	
private:
	Node* rayCast( const ci::Ray& ray );
	
	ci::Ray mRay;
	Node2d mRoot;
	Game* mGame;
	ci::Vec3f mLightPosition;
	std::list<Node2d*> mSprites;
};