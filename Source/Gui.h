#pragma once

#include "cinder/Vector.h"

#include "Node.h"
#include "ResourceManager.h"

class Gui {
public:
	Gui();
	virtual ~Gui();
	void update( const float deltaTime );
	
	void setTexture( Texture* texture );
	void setColor( ci::Vec4f color );
	
	Node* getNode() const { return mNode; }
	ci::Vec2i getGlobalPosition();
	
	ci::Vec2f anchor;
	ci::Vec2i size;
	ci::Vec2i position;
	float rotation;
	
private:
	Gui* mParent;
	Node* mNode;
};