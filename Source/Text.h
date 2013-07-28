#pragma once

#include "Node.h"

#include "cinder/Vector.h"

#include <string>
#include <vector>

/**
 A basic text box or line of text.  Primary function is to arrange a series of 'Font::Character'
 objects according to a string passed in through the 'setText' method.
 */
class Text {
public:
	typedef enum { LEFT, CENTER, RIGHT } Alignment_t;
	
	/** Object containing text properties to be applied when text is set either through
	 constructor 'Text::Text' or 'Text::setText' */
	class Options {
	public:
		Options();
		ci::Vec4f			color;
		int					letterSpacing;
		int					lineSpacing;
		float				scale;
		bool				truncateWithDots;
		int					width;
		int					height;
		int					padding;
		Text::Alignment_t	alignment;
	};
	
	Text();
	
	/** Main constructor */
	Text( Font* font, Text::Options& options, std::string text = "" );
	virtual ~Text();
	
	/** Sets text and applies options as set through constructor or 'setOptions' method */
	void setText( const std::string& text );
	
	/** Sets the text display options, will be applied next time text is set.  It is done this
	 *	way so that the text string, which could be very long, needn't be copied and stored here.
	 *  So where the text string lives, it should be re-set after options have been updated. */
	void setOptions( Options& options );
	
	/** Conveneince method for renderer to grab color */
	ci::Vec4f getColor() const { return  mOptions.color; }
	
	Font* getFont() const { return mFont; }
	
	Texture* getTexture() const { return mTexture; }
	
	/** Characters are drawn on a 2-triangle plane */
	Mesh* getMesh() const { return mMesh; }

	/** Returns reference to vector of characters configured with 'mOptions' and created by setting the text */
	const std::vector<Font::Character*>& getCharacters() { return mCharacters; }
	
	ci::Vec2f position;
	ci::Vec2i size;
	
private:
	void clearCharacters();
	void revertToLastSpace( const std::string& text, int& i );
	void insertCharacter( const char character );
	void removeLastCharacter();
	int getCharacterWidth( const char character );
	
	ci::Vec2f mPenPosition;
	Options mOptions;
	Mesh* mMesh;
	Texture* mTexture;
	Font* mFont;
	std::vector<Font::Character*> mCharacters;
};