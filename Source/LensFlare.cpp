#include "LensFlare.h"
#include "Game.h"
#include "Camera.h"

using namespace ci;

const static int kNumSprites = 9;

LensFlare::LensFlare( Game* game ) : mGame( game )
{
	LensFlare::Sprite sprites[ kNumSprites ];
	sprites[0] = (Sprite) { 200,		Vec4f(0.48,.35,.22,0.5),	"textures/flare_sprite_1.png" };
	sprites[1] = (Sprite) { 600,		Vec4f(1,1,1,1),				"textures/flare_sprite_0.png" };
	sprites[2] = (Sprite) { 50,			Vec4f(1,.2,.2,.5),			"textures/flare_sprite_1.png" };
	sprites[3] = (Sprite) { 20,			Vec4f(1,1,1,.5),			"textures/flare_sprite_2.png" };
	sprites[4] = (Sprite) { 100,		Vec4f(1,.2,.2,.5),			"textures/flare_sprite_1.png" };
	sprites[5] = (Sprite) { 80,			Vec4f(.2,.2,1,.5),			"textures/flare_sprite_1.png" };
	sprites[6] = (Sprite) { 30,			Vec4f(.2,.2,1,.5),			"textures/flare_sprite_1.png" };
	sprites[7] = (Sprite) { 200,		Vec4f(1,1,1,.25),			"textures/flare_sprite_3.png" };
	sprites[8] = (Sprite) { 300,		Vec4f(1,1,1,.1),			"textures/flare_sprite_4.png" };
	
	for( int i = 0; i < kNumSprites; i++ ) {
		Node2d* sprite = new Node2d();
		sprite->setTexture( ResourceManager::get()->getTexture( sprites[i].texture ) );
		sprite->size = Vec2i( sprites[i].size, sprites[i].size );
		sprite->setColor( sprites[i].color );
		sprite->anchor = Vec2f( 0.5f, 0.5f );
		mRoot.addChild( sprite );
		mSprites.push_back( sprite );
	}
	mLightPosition = Vec3f( 100.0, 50.0, 50.0 );
	
	mGame->getRenderingEngine()->addNode( &mRoot );
}

LensFlare::~LensFlare()
{
	while( !mSprites.empty() ) {
		delete *mSprites.begin();
		mSprites.erase( mSprites.begin() );
	}
}

void LensFlare::debugDraw()
{
	mGame->getRenderingEngine()->debugDrawLine( mRay.getOrigin(), mRay.getOrigin() + mRay.getDirection(), Vec4f(1,0,0,1) );
}

float angle = 0.0f;

void LensFlare::update( const float deltaTime )
{
	Camera* camera = Camera::get();
	
	const Vec2i start = camera->worldToScreen( mLightPosition );
	
	float zoom = camera->getZoom();
	camera->setZoom( zoom + 1 );
	
	/*if ( !camera->getScreenRect().contains( start ) ) {
		mRoot.hide();
	}
	else {*/
		const Vec2i center = camera->getScreenCenter();
		const Vec2i dir = start - center;
		const Vec2i end = center - dir;
		angle += 0.02f;
		Vec3f destination = Vec3f(0, math<float>::sin(angle) * 2.0f, 0.0f );
		Vec3f origin = mLightPosition;
		mRay = Ray( origin, destination - origin );
		bool rayHit = mGame->rayCast( mRay );
		if ( !rayHit ) {
			int i = -1;
			int total = kNumSprites;
			for( auto iter = mSprites.begin(); iter != mSprites.end(); iter++ ) {
				float r = (float) i++ / (float) total;
				(*iter)->position = start + r * ( end - start );
				(*iter)->update();
			}
			mRoot.show();
		}
		else {
			mRoot.hide();
		}
	//}
}