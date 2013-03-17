#pragma once

#pragma once

#include "cinder/Vector.h"
#include "cinder/Matrix.h"

#include <vector>
	
class Touch {
public:
	Touch();
	~Touch();
	static ci::Vec2f		getTouchesCenter( const std::vector<ci::Vec2i>& touches);
	static float			getDistance( const std::vector<ci::Vec2i>& touches);
	static ci::Vec2f		getScale( ci::Matrix44f& matrix );
	
};