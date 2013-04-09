#pragma once

#include "ResourceLoader.h"
#include "RenderingEngine.h"
#include "ShaderProgram.h"
#include "Node.h"

#include <map>
#include <list>

class ResourceManager {
public:
	virtual ~ResourceManager();
	static ResourceManager* get();
	
	void					setup( RenderingEngine* renderingEngine );
	
	void					loadShader( std::string key, std::string vertexShaderPath, std::string fragmentShaderPath );
	void					loadTexture( std::string filePath );
	void					loadMesh( std::string filePath );
	void					loadFont( std::string fontpath );
	
	ShaderProgram*			getShader( std::string key );
	Texture*				getTexture( std::string key );
	Mesh*					getMesh( std::string key );
	Font*					getFont( std::string key );
	
private:
	static ResourceManager* sInstance;
	RenderingEngine*		mRenderingEngine;
	ResourceLoader			mResourceLoader;
	
	std::map<std::string, Texture*> mTextures;
	std::map<std::string, Mesh*> mMeshes;
	std::map<std::string, ShaderProgram*> mShaders;
	std::map<std::string, Font*> mFonts;
};