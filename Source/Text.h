#pragma once

#include "RenderingEngine.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class Text {
public:
	Text();
	virtual ~Text();
	Texture* drawToTexture();
	
private:
	ResourceLoader mResourceLoader;
};