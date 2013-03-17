#include "Gui.h"
#include "GameConstants.h"

using namespace ci;

Gui::Gui() : mParent( NULL ), mIsVisible( true ), mIsEnabled( true ), zIndex( 0 ), tag(-1), group(-1)
{
	mNode = new Node();
	mNode->mShader = kShaderGui2d;
	position = Vec2i::zero();
	size = Vec2i::zero();
	rotation = 0.0f;
	anchor = Vec2i::zero();
	childAnchor = Vec2i::zero();
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
		const Vec2f& a = getParent()->childAnchor;
		p = mParent->getGlobalPosition();
		if ( a != Vec2f::zero() ) {
			p -= Vec2i( getParent()->size.x * (0.5f - a.x), getParent()->size.x * (0.5f - a.y) );
		}
	}
	return p + position;
}

void Gui::update( const float deltaTime )
{
	// Check if zIndex of a child has changed, which means we need to sort
	for( std::vector<Gui*>::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
		if ( (*iter)->zIndex.hasChanged() ) {
			std::sort( mChildren.begin(), mChildren.end(), Gui::sortByZIndex );
			break;
		}
	}
	
	for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
		(*iter)->update( deltaTime );
	}
	
	const Vec2i p		= getGlobalPosition();
	mNode->scale.x		= size.x;
	mNode->scale.y		= size.y;
	mNode->position.x	= mNode->scale.x * (0.5f - anchor.x) + p.x;
	mNode->position.y	= -mNode->scale.y * (0.5f - anchor.y) + p.y;
	mNode->rotation.z	= rotation;
	
	mNode->update( deltaTime );
}

void Gui::hide( bool andChildren )
{
	if ( !mIsVisible ) return;
	mIsVisible = false;
	
	if ( andChildren ) {
		for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
			(*iter)->hide( andChildren );
		}
	}
}

void Gui::show( bool andChildren )
{
	if ( mIsVisible ) return;
	mIsVisible = true;
	
	if ( andChildren ) {
		for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
			(*iter)->show( andChildren );
		}
	}
}

void Gui::enable()
{
	for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
		(*iter)->enable();
	}
	mIsEnabled = true;
}

void Gui::disable()
{
	for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
		(*iter)->disable();
	}
	mIsEnabled = false;
}

void Gui::addChild( Gui* gui )
{
	if ( !hasChild( gui ) ) {
		mChildren.push_back( gui );
		gui->zIndex = mChildren.size()-1;
		gui->setParent( this );
	}
}

void Gui::removeAllChildren( bool cleanup )
{
	while( !mChildren.empty() ) {
		(*mChildren.begin())->clearParent();
		if ( cleanup ) {
			delete *mChildren.begin();
		}
		mChildren.erase( mChildren.begin() );
	}
}

void Gui::removeChild( Gui* gui )
{
	auto existing = std::find( mChildren.begin(), mChildren.end(), gui );
	if ( existing != mChildren.end() ) {
		(*existing)->clearParent();
		mChildren.erase( existing );
	}
}

void Gui::removeChild( int index )
{
	if ( index >= 0 && index < getNumChildren() ) {
		removeChild( mChildren[ index ] );
	}
}

bool Gui::hasChild( Gui* gui )
{
	auto existing = std::find( mChildren.begin(), mChildren.end(), gui );
	return existing != mChildren.end();
}

void Gui::setParent( Gui* gui )
{
	mParent = gui;
}

void Gui::clearParent()
{
	mParent = NULL;
}