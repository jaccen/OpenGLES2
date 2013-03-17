#pragma once

#include "cinder/Vector.h"

#include <vector>

#include "Node.h"
#include "ResourceLoader.h"
#include "SmartValue.h"

class Gui {
public:
	
	static bool sortByZIndex( Gui* a, Gui* b ) { return a->zIndex < b->zIndex; }
	
	struct FindByTagGroup {
		FindByTagGroup( int group, int tag ) : mTag( tag ), mGroup( group ) {}
		int mTag;
		int mGroup;
		bool operator()( Gui* gui ) { return gui->tag == mTag && gui->group == mGroup; }
	};
	
	Gui();
	virtual ~Gui();
	void update( const float deltaTime );
	
	void setTexture( Texture* texture );
	void setColor( ci::Vec4f color );
	
	void hide( bool andChildren = true );
	void show( bool andChildren = true );
	bool getIsVisible() const { return mIsVisible; }
	
	void addChild( Gui* gui );
	void removeChild( Gui* gui );
	void removeChild( int index );
	bool hasChild( Gui* gui );
	void setParent( Gui* gui );
	Gui* getParent() { return mParent; }
	void clearParent();
	void removeAllChildren( bool cleanup = true );
	const std::vector<Gui*>& getChildren() const { return mChildren; }
	std::vector<Gui*>& getChildren() { return mChildren; }
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
	std::vector<Gui*> mChildren;
	Gui* mParent;
	Node* mNode;
	
	int tag;
	int group;
	bool mIsEnabled;
	bool mIsVisible;
};