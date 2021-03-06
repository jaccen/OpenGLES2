#pragma once

const static std::string kFontChars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

const static float kToRad = 0.017453292519943295769f;
const static float kToDeg = 57.295779513082321f;

static inline float randFloat( float min = 0.0f, float max = 1.0f )
{
	int r = (int)min * 100 + arc4random() % (int)((max-min) * 100);
	return (float)r / 100.0f;
}

static inline ci::Vec3f randVec3f( float min = 0.0f, float max = 1.0f )
{
	return ci::Vec3f( randFloat(min,max), randFloat(min,max), randFloat(min,max) );
}

static inline ci::Vec3f randVec3fSphere( float min = 0.0f, float max = 1.0f )
{
	return randVec3f(min,max).normalized();
}