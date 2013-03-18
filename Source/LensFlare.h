#pragma once

#include "cinder/Vector.h"

#include <list>

#include "Gui.h"

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
	Gui mRoot;
	Game* mGame;
	ci::Vec3f mLightPosition;
	std::list<Gui*> mSprites;
};