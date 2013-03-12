#include "ResourceManager.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

std::string ResourceManager::GetResourcePath() const
{
	NSString* bundlePath = [[NSBundle mainBundle] resourcePath];
	return [bundlePath UTF8String];
}

void ResourceManager::LoadPngImage(const std::string& name)
{
    CFDataRef m_imageData;
	
	NSString* basePath = [[NSString alloc] initWithUTF8String:name.c_str()];
	NSBundle* mainBundle = [NSBundle mainBundle];
	NSString* fullPath = [mainBundle pathForResource:basePath ofType:@"png"];
	UIImage* uiImage = [[UIImage alloc] initWithContentsOfFile:fullPath];
	CGImageRef cgImage = uiImage.CGImage;
	m_imageSize.x = CGImageGetWidth(cgImage);
	m_imageSize.y = CGImageGetHeight(cgImage);
	m_imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
	[uiImage release];
	[basePath release];
}

/*void* ResourceManager::GetImageData()
{
	return (void*) CFDataGetBytePtr(m_imageData);
}

ivec2 ResourceManager::GetImageSize()
{
	return m_imageSize;
}

void ResourceManager::UnloadImage()
{
	CFRelease(m_imageData);
}*/