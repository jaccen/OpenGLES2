#include "LifeMeter.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "ObjectController.h"
#include "Camera.h"

#include "cinder/Color.h"

using namespace ci;

LifeMeter::LifeMeter()
{
	Vec2i s = Vec2i( 30, 3 );
	
	mBackground.size = s;
	mBackground.position.x = -s.x / 2;
	addChild( &mBackground );
	
	mBar.size = s;
	mBar.size.x /= 5;
	mBar.position.x = -s.x / 2;
	addChild( &mBar );
	
	mIcon.setTexture( ResourceManager::get()->getTexture( "textures/icon.png" ) );
	mIcon.anchor = Vec2f( 0.5f, 1.2f );
	addChild( &mIcon );
	
	size = s;
	RenderingEngine::get()->addNode( this );
}

void LifeMeter::update( const float detlaTime )
{
	position = Camera::get()->getWorldToScreen( mOwner->getNode()->getGlobalPosition() );
}

void LifeMeter::setOwner( ObjectController* owner )
{
	mOwner = owner;
	
	/*Vec3f color = owner->mFaction == 0 ? Vec3f( 0, 1, 0 ) : Vec3f( 1, 0, 0 );
	mBackground.setColor( ColorA( color, 0.2f ) );
	mBar.setColor( ColorA( color, 0.7f ) );
	mIcon.setColor( ColorA( color, 0.5f ) );*/
}

LifeMeter::~LifeMeter()
{
}