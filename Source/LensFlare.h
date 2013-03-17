#pragma once

#include "cinder/Vector.h"

#include "Gui.h"

class Game;

class LensFlare {
public:
	LensFlare( Game* game );
	virtual ~LensFlare();
	
private:
	Game* mGame;
};