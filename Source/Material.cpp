#include "Material.h"

using namespace ly;
using namespace ci;

PostFilterMaterial::PostFilterMaterial() : Material(), bindDepthMap( false ), bindNormalhMap( false ), bindBlurTexture( false ) {}

void Material::setTexture( const std::string& location, Texture* texture )
{
	mTextures[ location ] = texture;
}

void Material::setColor( const std::string& location, const ci::ColorA& color )
{
	mColors[ location ] = color;
}

void Material::setColor( const std::string& location, const ci::Color& color )
{
	ColorA colorA = ColorA( color, 1.0f );
	mColors[ location ] = colorA;
}

void Material::setProperty( const std::string& location, float value )
{
	mProperties[ location ] = value;
}