#include "Node2d.h"
#include "GameConstants.h"
#include "ResourceManager.h"

using namespace ci;

Node2d::Node2d() : mParent( NULL ), mIsVisible( true ), mIsEnabled( true ), zIndex( 0 ), tag(-1), group(-1)
{
	mNode = new Node();
	position = Vec2i::zero();
	size = Vec2i::zero();
	rotation = 0.0f;
	anchor = Vec2i::zero();
	childAnchor = Vec2i::zero();
	mNode->mShader = kShaderScreenSpace;
	mNode->mMesh = ResourceManager::get()->getMesh( "models/quad_plane.obj" );
}

Node2d::~Node2d()
{
	delete mNode;
}

void Node2d::setTexture( Texture* texture )
{
	mNode->mTexture = texture;
}
void Node2d::setColor( ci::Vec4f color )
{
	mNode->mColor = color;
}

ci::Vec2i Node2d::getGlobalPosition()
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

void Node2d::update( const float deltaTime )
{
	// Check if zIndex of a child has changed, which means we need to sort
	for( std::vector<Node2d*>::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
		if ( (*iter)->zIndex.hasChanged() ) {
			std::sort( mChildren.begin(), mChildren.end(), Node2d::sortByZIndex );
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

void Node2d::hide( bool andChildren )
{
	if ( !mIsVisible ) return;
	mIsVisible = false;
	
	if ( andChildren ) {
		for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
			(*iter)->hide( andChildren );
		}
	}
}

void Node2d::show( bool andChildren )
{
	if ( mIsVisible ) return;
	mIsVisible = true;
	
	if ( andChildren ) {
		for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
			(*iter)->show( andChildren );
		}
	}
}

void Node2d::enable()
{
	for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
		(*iter)->enable();
	}
	mIsEnabled = true;
}

void Node2d::disable()
{
	for( auto iter = mChildren.begin(); iter != mChildren.end(); iter++ ) {
		(*iter)->disable();
	}
	mIsEnabled = false;
}

void Node2d::addChild( Node2d* node )
{
	if ( !hasChild( node ) ) {
		mChildren.push_back( node );
		node->zIndex = mChildren.size()-1;
		node->setParent( this );
	}
}

void Node2d::removeAllChildren( bool cleanup )
{
	while( !mChildren.empty() ) {
		(*mChildren.begin())->clearParent();
		if ( cleanup ) {
			delete *mChildren.begin();
		}
		mChildren.erase( mChildren.begin() );
	}
}

void Node2d::removeChild( Node2d* Node2d )
{
	auto existing = std::find( mChildren.begin(), mChildren.end(), Node2d );
	if ( existing != mChildren.end() ) {
		(*existing)->clearParent();
		mChildren.erase( existing );
	}
}

void Node2d::removeChild( int index )
{
	if ( index >= 0 && index < getNumChildren() ) {
		removeChild( mChildren[ index ] );
	}
}

bool Node2d::hasChild( Node2d* Node2d )
{
	auto existing = std::find( mChildren.begin(), mChildren.end(), Node2d );
	return existing != mChildren.end();
}

void Node2d::setParent( Node2d* Node2d )
{
	mParent = Node2d;
}

void Node2d::clearParent()
{
	mParent = NULL;
}