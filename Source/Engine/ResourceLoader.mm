#include "ResourceLoader.h"

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <fstream>
#import <sstream>

std::string ResourceLoader::getResourcePath() const
{
	NSString* bundlePath = [[NSBundle mainBundle] resourcePath];
	return [bundlePath UTF8String];
}

char* ResourceLoader::loadShader( const std::string& path )
{
	NSString* basePath = [NSString stringWithUTF8String:path.c_str()];
	NSString* extesion = [basePath pathExtension];
	basePath = [basePath stringByDeletingPathExtension];
	NSString* fullPath = [[NSBundle mainBundle] pathForResource:basePath
													 ofType:extesion];
	
	NSString* content = [NSString stringWithContentsOfFile:fullPath
												  encoding:NSUTF8StringEncoding
													 error:NULL];
	
	int length = content.length;
	size_t size = sizeof(char*) * length;
	char* output = (char*) malloc( size );
	std::memcpy( (void*) output, [content cStringUsingEncoding:NSASCIIStringEncoding], size );
	return output;
}

void ResourceLoader::loadImage( const std::string& path, void** pixelDataDestination, int* width, int* height )
{
	NSString* basePath = [NSString stringWithUTF8String:path.c_str()];
	NSBundle* mainBundle = [NSBundle mainBundle];
	NSString* extesion = [basePath pathExtension];
	basePath = [basePath stringByDeletingPathExtension];
	NSString* fullPath = [mainBundle pathForResource:basePath ofType:extesion];
	if ( fullPath == nil ) {
		std::cout << "Error loading image '" << path << "' -> Does not exist." << std::endl;
		return;
	}
	UIImage* uiImage = [[UIImage alloc] initWithContentsOfFile:fullPath];
	CGImageRef cgImage = uiImage.CGImage;
	CFDataRef imageData = CGDataProviderCopyData( CGImageGetDataProvider( cgImage ) );
	
	int imgWidth = CGImageGetWidth( cgImage );
	int imgHeight = CGImageGetHeight( cgImage );
	
	size_t size = imgWidth * imgHeight * 4 * sizeof(GLubyte);
	*pixelDataDestination = (void*) malloc( size );
	memcpy( *pixelDataDestination, CFDataGetBytePtr( imageData ), size );
	
	*width = imgWidth;
	*height = imgHeight;
	
	CFRelease( imageData );
	[uiImage release];
}

std::string ResourceLoader::loadTextFile( const std::string filepath )
{
	NSString* basePath = [NSString stringWithUTF8String:filepath.c_str()];
	NSString* extesion = [basePath pathExtension];
	basePath = [basePath stringByDeletingPathExtension];
	NSString* fullPath = [[NSBundle mainBundle] pathForResource:basePath
														 ofType:extesion];
	
	NSString* content = [NSString stringWithContentsOfFile:fullPath
												  encoding:NSUTF8StringEncoding
													 error:NULL];
	
	std::string output( [content UTF8String] );
	return output;
}

bool ResourceLoader::loadAudioFile( const std::string& path, unsigned long* sizeBytes, void** data )
{
	NSString* basePath = [NSString stringWithUTF8String:path.c_str()];
	NSString* extesion = [basePath pathExtension];
	basePath = [basePath stringByDeletingPathExtension];
	NSString* fullPath = [[NSBundle mainBundle] pathForResource:basePath
														 ofType:extesion];
	AudioFileID audioFileId;
	NSURL* fileUrl = [NSURL fileURLWithPath:fullPath];
	OSStatus openResult = AudioFileOpenURL( (__bridge CFURLRef)fileUrl, kAudioFileReadPermission, 0, &audioFileId );
	
	if ( openResult != 0 ) {
		std::cout << "Error loading audio file " << path << ": " << openResult << std::endl;
		return false;
	}
	
	UInt64 fileSizeBytes = 0;
	UInt32 propSize = sizeof( fileSizeBytes );
	
	OSStatus getSizeResult = AudioFileGetProperty( audioFileId, kAudioFilePropertyAudioDataByteCount, &propSize, &fileSizeBytes );
	if ( getSizeResult != 0 ) {
		std::cout << "Error getting size of audio file: " << path << std::endl;
		return false;
	}
	
	*sizeBytes = (UInt32) fileSizeBytes;
	*data = malloc( *sizeBytes );
	OSStatus readBytesResult = AudioFileReadBytes( audioFileId, false, 0, sizeBytes, *data );
	if ( readBytesResult != 0 ) {
		std::cout << "Error reading audio data for file: " << path << std::endl;
		return false;
	}
	
	AudioFileClose( audioFileId );
	
	return true;
}