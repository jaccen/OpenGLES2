#include "Projectile.h"
#include "RenderingEngine.h"

using namespace ci;

Projectile::Projectile() : mSpeed( 20.0f )
{
	mNode.addDelegate( this );
	mForward = Vec3f::zAxis();
	RenderingEngine::get()->addSpriteNode( &mNode );
}

Projectile::~Projectile() {}

void Projectile::update( const float deltaTime )
{
	mNode.position += mForward * mSpeed * deltaTime;
}