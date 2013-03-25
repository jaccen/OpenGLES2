#pragma once

#include "Node2d.h"

#include "cinder/Vector.h"

class ObjectController;

class LifeMeter : public Node2d {
public:
	LifeMeter();
	virtual ~LifeMeter();
	void setOwner( ObjectController* owner );
	void update( const float deltaTime );
	
private:
	ObjectController* mOwner;
	Node2d mBar;
	Node2d mBackground;
	Node2d mIcon;
};