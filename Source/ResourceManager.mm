#include "ResourceManager.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <fstream>
#import <sstream>

Texture::Texture( void* imageData, ci::Vec2i imageSize )
{
	mImageSize = imageSize;
	mImageData = imageData;
}

Texture::~Texture()
{
	free( mImageData );
}

std::string ResourceManager::getResourcePath() const
{
	NSString* bundlePath = [[NSBundle mainBundle] resourcePath];
	return [bundlePath UTF8String];
}

const char* ResourceManager::loadShader( const std::string& path )
{
	// TODO: GLSL Include stuff
	std::string fullPath = getResourcePath() + "/" + path;
	std::ifstream file( fullPath.c_str());
	if ( !file ) {
		std::cout << "ERROR: Could not load shader: File not found: " << fullPath << std::endl;
		exit(1);
	}
	else {
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string text = buffer.str();
		//std::cout << std::endl << text << std::endl << std::endl;
		const char* output = (const char*) malloc( sizeof(char) * text.length() );
		std::memcpy( (void*) output, text.c_str(), sizeof(char) * text.length() );
		return output;
	}
	std::cout << "ERROR: File found, but could not be loaded." << fullPath << std::endl;
	exit(1);
	return NULL;
}

Texture* ResourceManager::loadImage( const std::string& path )
{
	NSString* basePath = [NSString stringWithUTF8String:path.c_str()];
	NSBundle* mainBundle = [NSBundle mainBundle];
	NSString* extesion = [basePath pathExtension];
	basePath = [basePath stringByDeletingPathExtension];
	NSString* fullPath = [mainBundle pathForResource:basePath ofType:extesion];
	if ( fullPath == nil ) {
		std::cout << "Error loading image: Does not exist." << std::endl;
		return NULL;
	}
	UIImage* uiImage = [[UIImage alloc] initWithContentsOfFile:fullPath];
	CGImageRef cgImage = uiImage.CGImage;
	CFDataRef imageData = CGDataProviderCopyData( CGImageGetDataProvider( cgImage ) );
	
	int width = CGImageGetWidth( cgImage );
	int height = CGImageGetHeight( cgImage );
	
	void* textureData = (void*) malloc( width * height * 4 * sizeof(GLubyte) );
	memcpy( textureData, CFDataGetBytePtr( imageData ), width * height * 4 * sizeof(GLubyte) );
	
	Texture* texture = new Texture( textureData, ci::Vec2i( width, height ) );
	
	CFRelease( imageData );
	[uiImage release];
	
	return texture;
}