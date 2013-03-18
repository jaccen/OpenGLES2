#include "ResourceManager.h"

#include "cinder/Vector.h"
#include "cinder/TriMesh.h"

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
		mTextures[ filePath ] = texture;
	}
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
		TriMesh mesh;
		for( auto iter = vertices.begin(); iter != vertices.end(); iter+=3 ) {
			vboMesh->indexOrderedVertices.push_back( Vec3f( *iter, *(iter+1), *(iter+2) ) );
		}
		mRenderingEngine->createVbo( vboMesh, vertices, normals, texCoords );
		mMeshes[ filePath ] = vboMesh;
		delete objParser;
	}
}

void ResourceManager::loadShader( std::string key, std::string vertexShaderPath, std::string fragmentShaderPath )
{
	char* vShader = mResourceLoader.loadShader( vertexShaderPath );
	char* fShader = mResourceLoader.loadShader( fragmentShaderPath );
	mRenderingEngine->addShader( key, vShader, fShader );
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
