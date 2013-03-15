#include "ResourceManager.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>


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

Texture* ResourceManager::loadImage( const std::string& name )
{
	NSString* basePath = [[NSString alloc] initWithUTF8String:name.c_str()];
	NSBundle* mainBundle = [NSBundle mainBundle];
	NSString* fullPath = [mainBundle pathForResource:basePath ofType:@"png"];
	if ( fullPath == nil ) {
		std::cout << "Error loading image: Does not exist." << std::endl;
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
	[basePath release];
	
	return texture;
}