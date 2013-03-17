#include "LensFlare.h"
#include "Game.h"

using namespace ci;

LensFlare::LensFlare( Game* game ) : mGame( game )
{
	Gui* sunSprite = new Gui();
	//sunSprite->getNode()->mMesh = planeMesh;
	//sunSprite->setTexture( sunSpriteTexture );
	sunSprite->getNode()->mShader = kShaderGui2d;
	sunSprite->size = Vec2i( 300, 300 );
	sunSprite->anchor = Vec2f( 0.5f, 0.5f );
	mGame->add( sunSprite );
}

LensFlare::~LensFlare() {}