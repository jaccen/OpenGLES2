#include "Text.h"
#include "GameConstants.h"
#include "ResourceManager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost::property_tree;

using namespace ci;

Text::Options::Options() :
	letterSpacing( 0 ),
	lineSpacing( 0 ),
	width( 0 ),
	height( 0 ),
	scale( 1.0f ),
	truncateWithDots( false ),
	padding( 0 ),
	alignment( Text::LEFT )
{
	color = Vec4f::one();
}

Text::Text() : mFont( NULL ), mOptions(), mTexture( NULL ) {}

Text::Text( Font* font, Text::Options& options, std::string text ) :
	mFont( font ),
	mOptions( options ),
	mTexture( NULL )
{
	mMesh = ResourceManager::get()->getMesh( "models/quad_plane.obj" );
	setText( text );
}

Text::~Text()
{
	// This will delete the elements in the list:
	clearCharacters();
}

void Text::setOptions( Text::Options& options )
{
	mOptions = options;
}

void Text::clearCharacters()
{
	// Clean up pointers and clear
	for( const auto c : mCharacters ) {
		delete c;
	}
	mCharacters.clear();
	
	// Delete texture
	if ( mTexture != NULL ) {
		RenderingEngine::get()->deleteTexture( mTexture );
		delete mTexture;
	}
}

void Text::setText( const std::string& text )
{
	// Start with a fresh crop of new characters
	clearCharacters();
	
	Options& o = mOptions;
	
	mPenPosition = Vec2i( 0, -mFont->defaultLineHeight * 0.6f );
	
	for( int i = 0; i < text.length(); i++ ) {
		// Inser the next character character
		insertCharacter( text[i] );
		
		// Check end of line according to 'width' property
		bool lineWidthExceeded = o.width > 0 && mPenPosition.x * o.scale > o.width - o.padding * 2;
		if ( lineWidthExceeded ) {
			
			// Check max lines according to 'height' property
			bool lineHeightExceeded = o.height > 0 && ( mPenPosition.y + mFont->defaultLineHeight ) * o.scale > o.height - o.padding * 2;
			if ( lineHeightExceeded ) {
				if ( o.truncateWithDots ) {
					// Go back to the most recent space
					revertToLastSpace( text, i );
					// Add the dots
					insertCharacter( '.' );
					insertCharacter( '.' );
					insertCharacter( '.' );
				}
				
				// Out of space, so stop adding characters
				break;
			}
			else {
				// Go back to the most recent space
				revertToLastSpace( text, i );
				// Insert new line
				insertCharacter( '\n' );
				// And add the next character
				insertCharacter( text[i] );
			}
		}
	}
	
	// Create an FBO with a texture
	RenderingEngine* renderer = RenderingEngine::get();
	int size = ci::math<int>::max(  mPenPosition.x, mPenPosition.y );
	mTexture = new Texture( size, size );
	renderer->createTexture( mTexture );
	FramebufferObject* fbo = new FramebufferObject( mTexture );
	renderer->createFbo( fbo );
	
	// Draw the Characters to the texture
	renderer->bindFrameBufferObject( fbo );
	renderer->drawText( this );
}

void Text::removeLastCharacter()
{
	// Reset the pen position to where it was before the character was added
	mPenPosition.x -= mCharacters.back()->xadvance + mOptions.letterSpacing;
	
	// Removal it and clean up, since the Character pointer is copied from the font
	delete mCharacters.back();
	mCharacters.pop_back();
}

void Text::insertCharacter( const char c )
{
	Options& o = mOptions;
	
	// Check for special characters
	if ( c == '\n' ) {
		// New line by offseting mPenPosition
		mPenPosition.x = 0;
		mPenPosition.y += mFont->defaultLineHeight + o.lineSpacing;
	}
	
	else if ( Font::Character* character = mFont->getCharacterCopy( c ) ) {
		// Adjust character display properties according to Text settings
		character->paddingOffset = Vec2i( o.padding, o.padding );
		character->scale = o.scale;
		character->setPosition( mPenPosition.x, mPenPosition.y );
		character->visible = true;
		
		// Add it to the list for rendering
		mCharacters.push_back( character );
		
		// Update current position of where new characters will be added
		mPenPosition.x += character->xadvance + o.letterSpacing;
	}
}

int Text::getCharacterWidth( const char c )
{
	// Returns the width of the character stored in the font
	// Note: Does not use 'getCharacterCopy' method here
	if ( Font::Character* character = mFont->getCharacter( c ) ) {
		return character->size.x;
	}
	return 0;
}

void Text::revertToLastSpace( const std::string& text, int& i )
{
	// Iterate back until we hit a space, removing characters and resetting i from the loop
	while( ( text[i] != ' ' && text[i] != '-' ) && i >= 0 ) {
		removeLastCharacter();
		i--;
	}
	// If the current character is now a space, remove it as well and iterate forward so
	// that the space doesn't get added again.
	if ( text[i] == ' ' ) {
		removeLastCharacter();
		i++;
	}
}