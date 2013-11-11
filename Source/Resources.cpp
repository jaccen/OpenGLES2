#include "Resources.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "GameConstants.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;

using namespace ci;

Texture::Texture( int width, int height ) : mFormat( GL_RGBA ), mImageData( NULL )
{
	setSize( width, height );
}

ci::Vec2i Texture::getScaledTextureSize() const
{
	return ci::Vec2i( mWidth, mHeight ) / RenderingEngine::get()->getContentScaleFactor();
}

void Texture::addCubemapTexture( Texture* texture )
{
	if ( mCubemapTextures.size() < 6 ) {
		mCubemapTextures.push_back( texture );
	}
}

void Texture::setSize( int width, int height )
{
	int sizes[9] = { 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };
	for ( int i = 1; i < 9; i++ ) {
		if ( width <= sizes[i] && width > sizes[i-1] ) {
			mWidth = sizes[i];
		}
		if ( height <= sizes[i] && height > sizes[i-1] ) {
			mHeight = sizes[i];
		}
	}
}

FramebufferObject::FramebufferObject( int width, int height ) : mTexture(NULL), mWidth( width ), mHeight( height ), mFormat( GL_RGBA )
{
	
}

FramebufferObject::FramebufferObject( Texture* texture ) : mTexture( texture ), mWidth( texture->mWidth ), mHeight( texture->mHeight ), mFormat( texture->mFormat )
{
	
}

Font::Character::Character( char charValue ) : index( 0 ), scale( 1.0f ), mChar( charValue ), visible( false )
{
}

Font::Character::Character( Character* original )
{
	mChar			= original->getChar();
	index			= original->index;
	textureSize		= original->textureSize;
	offset			= original->offset;
	positionOffset	= original->positionOffset;
	position		= original->position;
	xadvance		= original->xadvance;
	size			= original->size;
	scale			= original->scale;
}

void Font::Character::setPosition( int x, int y )
{
	position = Vec2i( x, y ) + positionOffset;
	updateTransforms();
}

void Font::Character::updateTransforms()
{
	mTransform = Matrix44f::identity();
	mTransform.translate( Vec3f( paddingOffset.x, paddingOffset.y, 0.0f ) + Vec3f( position.x + size.x * 0.5f, position.y + size.y * 0.5f, 0 ) * scale );
	mTransform.scale( Vec3f( size.x, size.y, 0 ) * scale );
	
	mSourceTransform = Matrix44f::identity();
	mSourceTransform.translate( Vec3f( (float) offset.x / (float) textureSize.x, (float) offset.y / (float) textureSize.y, 0.0f ) );
	mSourceTransform.scale( Vec3f( (float) size.x / (float) textureSize.x, (float) size.y / (float) textureSize.y, 0 ));
}

Font::Font( std::string filepath )
{
	ptree root;
	try {
		xml_parser::read_xml( filepath, root );
	} catch( ptree_error& error  ) {
		std::cout << "Error loading XML property tree: " << error.what() << std::endl;
		return;
	}
	
	try {
		std::string textureName = root.get_child( "font.pages.page.<xmlattr>.file" ).data();
		mTexture = ResourceManager::get()->getTexture( "fonts/" + textureName );
		mTextureSize = mTexture->getScaledTextureSize();
	}
	catch( ptree_error& error ) {
		std::cout << "Error pasing XML for property tree: " << error.what() << std::endl;
	}
	
	try {
		const ptree& characters = root.get_child( "font.chars" );
		int i = 0;
		for( auto const& v : characters ) {
			if ( v.first != "char" ) continue;
			
			int index			= i;
			int x				= v.second.get<int>( "<xmlattr>.x" );
			int y				= v.second.get<int>( "<xmlattr>.y" );
			int width			= v.second.get<int>( "<xmlattr>.width" );
			int height			= v.second.get<int>( "<xmlattr>.height" );
			int xadvance		= v.second.get<int>( "<xmlattr>.xadvance" );
			int xoffset			= v.second.get<int>( "<xmlattr>.xoffset" );
			int yoffset			= v.second.get<int>( "<xmlattr>.yoffset" );
			
			Character* c		= new Character( kFontChars[i] );
			c->index			= index;
			c->textureSize		= mTextureSize;
			c->offset			= Vec2i( x, y );
			c->positionOffset	= Vec2i( xoffset, yoffset );
			c->size				= Vec2i( width == 0 ? xadvance : width, height == 0 ? 1 : height );
			c->position			= Vec2i::zero();
			c->xadvance			= xadvance;
			
			mCharacters.push_back( c );
			i++;
		}
		
		// To get a default line height, we'll find the largest character and use its height (size.y)
		std::vector<Font::Character*> toSort = mCharacters;
		std::sort( toSort.begin(), toSort.end(), Font::Character::sortByHeight );
		defaultLineHeight = (*(toSort.end()-1))->size.y;
		
	}
	catch( ptree_error& error ) {
		std::cout << "Error pasing XML for property tree: " << error.what() << std::endl;
	}
}

Font::Character* Font::getCharacterCopy( const char c )
{
	if ( Character* character = getCharacter( c ) ) {
		return new Character( character );
	}
	return NULL;
}

Font::Character* Font::getCharacter( const char c )
{
	// kFontChars contains a string of all characters provided by each font in order,
	// so here we find the ordered index of the character by matching it in a search
	int index = kFontChars.find( c );
	
	// If the character is not found in the set contained in the font, it will be ignored
	// otherwise, add it to our container of characters
	if ( index != std::string::npos ) {
		// Copy the character from the font
		return mCharacters[ index ];
	}
	else return NULL;
}