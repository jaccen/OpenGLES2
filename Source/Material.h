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
	void setProperty( const std::string& location, ci::Vec2f value );
	void setProperty( const std::string& location, ci::Vec3f value );
	
	std::map<std::string, ci::ColorA> mColors;
	std::map<std::string, Texture*> mTextures;
	std::map<std::string, float> mProperties;
	std::map<std::string, ci::Vec2f> mPropertiesVec2;
	std::map<std::string, ci::Vec3f> mPropertiesVec3;
};
	
class PostFilterMaterial : public Material {
public:
	PostFilterMaterial();
	bool bindDepthMap;
	bool bindNormalhMap;
	bool bindBlurTexture;
};
