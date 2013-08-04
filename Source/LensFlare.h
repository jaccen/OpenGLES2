#pragma once

#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Ray.h"

#include <list>

#include "Node2d.h"

class Game;

class LensFlare {
public:
	struct Sprite {
		int size;
		ci::ColorA color;
		std::string texture;
	};
	
	LensFlare();
	virtual ~LensFlare();
	
	void debugDraw();
	void update( const float deltaTime = 0.0f );
	void setLightPosition( const ci::Vec3f lightPosition ) { mLightPosition = lightPosition; }
	
private:
	Node* rayCast( const ci::Ray& ray );
	
	ci::Ray mRay;
	Node2d mRoot;
	ci::Vec3f mLightPosition;
	std::list<Node2d*> mSprites;
};