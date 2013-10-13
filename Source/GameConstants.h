#pragma once

const static std::string kFontChars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

const static float kToRad = 0.017453292519943295769f;
const static float kToDeg = 0.017453292519943295769;

static inline float randFloat( float min = 0.0f, float max = 1.0f )
{
	int r = (int)min * 100 + arc4random() % (int)((max-min) * 100);
	return (float)r / 100.0f;
}