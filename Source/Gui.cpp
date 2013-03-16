#include "Gui.h"
#include "GameConstants.h"

using namespace ci;

Gui::Gui() : mParent( NULL )
{
	mNode = new Node();
	mNode->mShader = kShaderGui2d;
	position = Vec2i::zero();
	size = Vec2i::zero();
	rotation = 0.0f;
	anchor = Vec2i::zero();
}

Gui::~Gui()
{
	delete mNode;
}

void Gui::setTexture( Texture* texture )
{
	mNode->mTexture = texture;
}
void Gui::setColor( ci::Vec4f color )
{
	mNode->mColor = color;
}

ci::Vec2i Gui::getGlobalPosition()
{
	Vec2i p = Vec2i::zero();
	if ( mParent != NULL ) {
		const Vec2f& a = mParent->anchor;
		p = mParent->getGlobalPosition();
		if ( a != Vec2f::zero() ) {
			p -= Vec2i( mParent->size.x * a.x, mParent->size.y * a.y );
		}
	}
	return p + position;
}

void Gui::update( const float deltaTime )
{
	const float contentScaleFactor = 2.0f;
	
	const Vec2i p		= getGlobalPosition();
	mNode->scale.x		= size.x * contentScaleFactor;
	mNode->scale.y		= size.y * contentScaleFactor;
	mNode->position.x	= mNode->scale.x * (0.5f - anchor.x) + p.x * contentScaleFactor;
	mNode->position.y	= -mNode->scale.y * (0.5f - anchor.y) + p.y * contentScaleFactor;
	mNode->rotation.z	= rotation;
	
	mNode->update( deltaTime );
}