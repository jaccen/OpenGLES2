#pragma once

#include "cinder/Color.h"

#include "Node.h"
#include "LensFlare.h"

class Light {
public:
	Light() : mAmbientColor( ci::ColorA::black() ), mColor( ci::ColorA::white() ) {}
	void update( const float deltaTime );
	ci::ColorA mColor;
	ci::ColorA mAmbientColor;
	Node mNode;
	LensFlare mLensFlare;
};