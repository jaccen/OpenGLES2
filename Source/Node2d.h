#pragma once

#include "cinder/Vector.h"

#include <vector>

#include "Node.h"
#include "ResourceLoader.h"
#include "SmartValue.h"

class Node2d {
public:
	
	static bool sortByZIndex( Node2d* a, Node2d* b ) { return a->zIndex < b->zIndex; }
	
	struct FindByTagGroup {
		FindByTagGroup( int group, int tag ) : mTag( tag ), mGroup( group ) {}
		int mTag;
		int mGroup;
		bool operator()( Node2d* Node2d ) { return Node2d->tag == mTag && Node2d->group == mGroup; }
	};
	
	Node2d();
	virtual ~Node2d();
	void update( const float deltaTime = 0.0f );
	
	void setTexture( Texture* texture );
	void setColor( ci::Vec4f color );
	
	void hide( bool andChildren = true );
	void show( bool andChildren = true );
	bool getIsVisible() const { return mIsVisible; }
	
	void addChild( Node2d* Node2d );
	void removeChild( Node2d* Node2d );
	void removeChild( int index );
	bool hasChild( Node2d* Node2d );
	void setParent( Node2d* Node2d );
	Node2d* getParent() { return mParent; }
	void clearParent();
	void removeAllChildren( bool cleanup = true );
	const std::vector<Node2d*>& getChildren() const { return mChildren; }
	std::vector<Node2d*>& getChildren() { return mChildren; }
	bool hasChildren() const { return !mChildren.empty(); }
	bool getNumChildren() const { return hasChildren() ? mChildren.size() : 0; }
	
	bool getIsEnabled() const { return mIsEnabled; }
	void enable();
	void disable();
	
	Node* getNode() const { return mNode; }
	ci::Vec2i getGlobalPosition();
	
	ci::Vec2f anchor;
	ci::Vec2f childAnchor;
	ci::Vec2i size;
	ci::Vec2i position;
	float rotation;
	
private:
	ly::ChangeValue<int> zIndex;
	std::vector<Node2d*> mChildren;
	Node2d* mParent;
	Node* mNode;
	
	int tag;
	int group;
	bool mIsEnabled;
	bool mIsVisible;
};