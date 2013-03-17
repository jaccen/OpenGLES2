#include "Touch.h"

using namespace ci;

Touch::Touch()
{
	
}

Touch::~Touch() {}

Vec2f Touch::getTouchesCenter( const std::vector<ci::Vec2i>& touches)
{
	Vec2f total = Vec2f::zero();
	for( std::vector<ci::Vec2i>::const_iterator iter = touches.begin(); iter != touches.end(); iter++ )
		total = total + *iter;
	Vec2f output = total / (float) touches.size();
	return output;
}

float Touch::getDistance( const std::vector<ci::Vec2i>& touches)
{
	float greatestDistance = 0.0f;
	for( std::vector<ci::Vec2i>::const_iterator iter_a = touches.begin(); iter_a != touches.end(); iter_a++ ) {
		for( std::vector<ci::Vec2i>::const_iterator iter_b = touches.begin(); iter_b != touches.end(); iter_b++ ) {
			float distance = iter_b->distance( *iter_a );
			if ( distance > greatestDistance ) {
				greatestDistance = distance;
			}
		}
	}
	return greatestDistance;
}

ci::Vec2f Touch::getScale( ci::Matrix44f& matrix )
{
	Vec2f scale = Vec2f::zero();
	Vec3f columns[2] = {
		matrix.getColumn(0).xyz(),
		matrix.getColumn(1).xyz()
	};
	scale.x = columns[0].length();
	scale.y = columns[1].length();
	return scale;
}