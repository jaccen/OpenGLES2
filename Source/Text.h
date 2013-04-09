#pragma once

#include "Node.h"

#include "cinder/Vector.h"

#include <string>
#include <vector>

class Text {
public:
	Text();
	Text( Font* font = NULL, std::string text = "" );
	virtual ~Text();
	void setText( const std::string& text );
	
	Font* getFont() const { return mFont; }
	Mesh* getMesh() const { return mMesh; }

	const std::vector<Font::Character*>& getCharacters() { return mCharacters; }
	
	ci::Vec4f mColor;
	int letterSpacing;
	int lineSpacing;
	float scale;
	bool truncateWithDots;
	int width;
	int height;
	int padding;
	
private:
	void clearCharacters();
	void revertToLastSpace( const std::string& text, int& i );
	void insertCharacter( const char character );
	void removeLastCharacter();
	int getCharacterWidth( const char character );
	
	ci::Vec2f mPenPosition;
	
	Mesh* mMesh;
	Font* mFont;
	std::vector<Font::Character*> mCharacters;
};