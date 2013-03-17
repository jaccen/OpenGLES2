#pragma once

#include "ResourceLoader.h"
#include "RenderingEngine.h"
#include "ShaderProgram.h"
#include "Node.h"

#include <map>
#include <list>

class ResourceManager {
public:
	ResourceManager( RenderingEngine* renderingEngine );
	virtual ~ResourceManager();
	
	void					loadShader( std::string key, std::string vertexShaderPath, std::string fragmentShaderPath );
	void					loadTexture( std::string filePath );
	void					loadMesh( std::string filePath );
	
	ShaderProgram*			getShader( std::string key );
	Texture*				getTexture( std::string key );
	VboMesh*				getMesh( std::string key );
	
private:
	RenderingEngine*		mRenderingEngine;
	ResourceLoader			mResourceLoader;
	
	std::map<std::string, Texture*> mTextures;
	std::map<std::string, VboMesh*> mMeshes;
	std::map<std::string, ShaderProgram*> mShaders;
};