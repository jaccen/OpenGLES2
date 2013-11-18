#include "ResourceManager.h"
#include "cinder/Vector.h"
#include "cinder/TriMesh.h"
#include "Audio.h"

using namespace ci;

ResourceManager* ResourceManager::sInstance = NULL;

ResourceManager* ResourceManager::get()
{
	if ( !sInstance ) {
		sInstance = new ResourceManager();
	}
	return sInstance;
}

void ResourceManager::setup( RenderingEngine* renderingEngine )
{
	mRenderingEngine = renderingEngine;
}

ResourceManager::~ResourceManager() {}

void ResourceManager::loadTexture( std::string filePath )
{
	auto match = mTextures.find( filePath );
	if ( match == mTextures.end() ) {
		Texture* texture = new Texture();
		mResourceLoader.loadImage( filePath, &texture->mImageData, &texture->mWidth, &texture->mHeight );
		mRenderingEngine->createTexture( texture );
		std::cout << "Loading texture: " << filePath << ": " << texture->mWidth << ", " << texture->mHeight << std::endl;
		mTextures[ filePath ] = texture;
	}
}

void ResourceManager::loadCubemapTexture( std::string key, const std::vector<std::string>& filePaths )
{
	Texture* cubemap = new Texture();
	for( const auto path : filePaths ) {
		loadTexture( path );
		Texture* face = getTexture( path );
		cubemap->addCubemapTexture( face );
		if ( cubemap->mWidth == 0 ) {
			cubemap->mWidth = face->mWidth;
			cubemap->mHeight = face->mHeight;
		}
		else if ( face->mWidth != cubemap->mWidth ||
				 face->mHeight != cubemap->mHeight ) {
			std::cout << "Error loading cubemap " << key << ".  A face texture was not the right size (expecting " << cubemap->mWidth << "x" << cubemap->mHeight << ")." << std::endl;
		}
	}
	mRenderingEngine->createCubeTexture( cubemap );
	std::cout << "Loading cubemap " << key << std::endl;
	mTextures[ key ] = cubemap;
}

void ResourceManager::loadMesh( std::string filePath )
{
	auto match = mMeshes.find( filePath );
	if ( match == mMeshes.end() ) {
		ObjParser* objParser = new ObjParser( mResourceLoader.getResourcePath() + "/" + filePath );
		Mesh* vboMesh = new Mesh();
		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> texCoords;
		objParser->getVertices( vertices );
		objParser->getTexCoords( texCoords );
		objParser->getNormals( normals );
		std::vector<ci::Vec3f>& allVerts = objParser->getVertices();
		vboMesh->triMesh.appendVertices( allVerts.data(), allVerts.size() );
		std::vector<int>& vIndices = objParser->getVertexIndices();
		vboMesh->triMesh.appendIndices( (uint32_t*) vIndices.data(), vIndices.size() );
		mRenderingEngine->createVbo( vboMesh, vertices, normals, texCoords );
		mMeshes[ filePath ] = vboMesh;
		delete objParser;
	}
}

void ResourceManager::loadFont( std::string path )
{
	std::string filepath = mResourceLoader.getResourcePath() + "/" + path;
	std::cout << "Loading font: " << path << std::endl;
	Font* font = new Font( filepath );
	if ( font->checkDidLoad() ) {
		mFonts[ path ] = font;
	}
}

void ResourceManager::loadSound( std::string path )
{
	std::string filepath = mResourceLoader.getResourcePath() + "/" + path;
	Sound* sound = new Sound();
	if ( mResourceLoader.loadAudioFile( path, &sound->sizeBytes, &sound->data ) ) {
		if ( Audio::get()->createSound( sound ) ) {
			mSounds[ path ] = sound;
			std::cout << "Loaded sound: " << path << std::endl;
			return;
		}
		else {
			std::cout << "ERROR creating sound: " << path << std::endl;
		}
	}
	else {
		std::cout << "ERROR Loading sound: " << path << std::endl;
	}
	
	delete sound;
}

void ResourceManager::loadShader( std::string key, std::string vertexShaderPath, std::string fragmentShaderPath )
{
	char* vShader = mResourceLoader.loadShader( vertexShaderPath );
	char* fShader = mResourceLoader.loadShader( fragmentShaderPath );
	mShaders[ key] = new ShaderProgram( vShader, fShader );
	free( vShader );
	free( fShader );
}

ShaderProgram* ResourceManager::getShader( std::string key )
{
	auto match = mShaders.find( key );
	if ( match != mShaders.end() ) {
		return mShaders[ key ];
	}
	return NULL;
}

Texture* ResourceManager::getTexture( std::string filePath )
{
	auto match = mTextures.find( filePath );
	if ( match != mTextures.end() ) {
		return mTextures[ filePath ];
	} else {
		loadTexture( filePath );
		return mTextures[ filePath ];
	}
	return NULL;
}

Mesh* ResourceManager::getMesh( std::string filePath )
{
	auto match = mMeshes.find( filePath );
	if ( match != mMeshes.end() ) {
		return mMeshes[ filePath ];
	}
	else {
		loadMesh( filePath );
		return mMeshes[ filePath ];
	}
	return NULL;
}


Sound* ResourceManager::getSound( std::string filePath )
{
	auto match = mSounds.find( filePath );
	if ( match != mSounds.end() ) {
		return mSounds[ filePath ];
	}
	else {
		loadSound( filePath );
		return mSounds[ filePath ];
	}
	return NULL;
}

Font* ResourceManager::getFont( std::string filePath )
{
	auto match = mFonts.find( filePath );
	if ( match != mFonts.end() ) {
		return mFonts[ filePath ];
	}
	else {
		loadFont( filePath );
		return mFonts[ filePath ];
	}
	return NULL;
}

