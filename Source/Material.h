#pragma once

#include <string>
#include <map>

#include "cinder/Color.h"

#include "ShaderProgram.h"
#include "Resources.h"

class Material {
public:
	Material() : mShader( NULL ), enabled( true ) {}
	~Material() {}
	
	std::string name;
	bool enabled;
	ShaderProgram* mShader;
	
	void setTexture( const std::string& location, Texture* texture );
	void setColor( const std::string& location, const ci::ColorA& color );
	void setColor( const std::string& location, const ci::Color& color );
	void setProperty( const std::string& location, float color );
	
	std::map<std::string, ci::ColorA> mColors;
	std::map<std::string, Texture*> mTextures;
	std::map<std::string, float> mProperties;
};
	
class PostFilterMaterial : public Material {
public:
	PostFilterMaterial();
	bool bindDepthMap;
	bool bindNormalhMap;
	bool bindBlurTexture;
};
