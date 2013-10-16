#pragma once

#include "Node.h"

class Planet : public Node::IDelegate {
public:
	Planet( Node* node );
	virtual ~Planet();
	
	virtual void update( const float deltaTime );
	
private:
	Node* mNode;
};