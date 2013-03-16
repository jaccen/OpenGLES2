#pragma once

#include "cinder/Vector.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <string>

class Texture {
public:
	Texture( void* imageData, ci::Vec2i imageSize );
	~Texture();
	GLuint mHandle;
	void* getImageData() const { return mImageData; }
	ci::Vec2i getImageSize() const { return mImageSize; }
private:
	std::string mName;
	std::string mPath;
	ci::Vec2i mImageSize;
	void* mImageData;
};


class ResourceManager {
public:
    Texture* loadImage(const std::string& path);
    std::string getResourcePath() const;
	const char* loadShader( const std::string& path );
};