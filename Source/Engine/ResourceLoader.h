#pragma once

#include "cinder/Vector.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <AudioToolbox/AudioToolbox.h>

#include <string>

class ResourceLoader {
public:
	std::string		loadTextFile( const std::string filepath );
	void			loadImage( const std::string& path, void** pixelDataDestination, int* width, int* height );
    std::string		getResourcePath() const;
	char*			loadShader( const std::string& path );
	bool			loadAudioFile( const std::string& path, unsigned long* sizeBytes, void** data );
};