#pragma once

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/TriMesh.h"

#include "SmartValue.h"
#include "Resources.h"

#include <vector>

class Sound {
public:
	unsigned long sizeBytes;
	float length;
	void* data;
	unsigned int outputSource;
	unsigned int outputBuffer;
};

class Texture {
public:
	Texture( int width, int height );
	Texture() : mFormat( GL_RGBA ), mImageData( NULL ), mWidth(0), mHeight(0) {}
	void setSize( int width, int height );
	~Texture() { free( mImageData ); }
	
	ci::Vec2i getScaledTextureSize() const;
	bool isCubemap() const { return mCubemapTextures.size() == 6; }
	const std::vector<Texture*>& getCubemapTextures() const { return mCubemapTextures; }
	const void* getImageData() const { return mImageData; }
	void addCubemapTexture( Texture* texture );
	
	GLuint					mFormat;
	GLuint					mHandle;
	int						mWidth;
	int						mHeight;
	void*					mImageData;
	
private:
	std::vector<Texture*>	mCubemapTextures;
};

class FramebufferObject {
public:
	FramebufferObject( int width, int height );
	FramebufferObject( Texture* texture );
	Texture* mTexture;
	int mWidth;
	int mHeight;
	GLuint mFormat;
	GLuint mHandle;
	GLuint mColorRenderbuffer;
	GLuint mDepthRenderbuffer;
};

class Mesh {
public:
	Mesh() : vertexBuffer(-1), normalBuffer(-1), texCoordBuffer(-1), indexBuffer(-1) {}
	void					getTriangleVertices( size_t i, ci::Vec3f *a, ci::Vec3f *b, ci::Vec3f *c ) const;
    GLuint					vertexBuffer;
	int						vertexCount;
    GLuint					texCoordBuffer;
    int						texCoordCount;
    GLuint					normalBuffer;
    int						normalCount;
    GLuint					indexBuffer;
    int						indexCount;
	std::vector<ci::Vec3f>	indexOrderedVertices;
	ci::TriMesh				triMesh;
};

class Font {
public:
	Font( std::string filepath );
	~Font() { for( const auto c : mCharacters ) { delete c; } }
	
	class Character {
	public:
		static bool sortByHeight( Character* a, Character* b ) { return a->size.y < b->size.y; }
		
		Character( Character* original );
		Character( char charValue );
		void setPosition( int oX, int oY );
		int index;
		ci::Vec2i position;
		ci::Vec2i offset;
		int xadvance;
		ci::Vec2i positionOffset;
		ci::Vec2i textureSize;
		ci::Vec2i size;
		ci::Vec2i paddingOffset;
		bool isSpace() { return mChar == ' '; }
		bool isAlpha() { return isalpha( mChar ); }
		const char getChar() { return mChar; }
		float scale;
		bool visible;
		const ci::Matrix44f& getTransform() { return mTransform; }
		const ci::Matrix44f& getSourceTransform() { return mSourceTransform; }
		
	private:
		char mChar;
		void updateTransforms();
		ci::Matrix44f mTransform;
		ci::Matrix44f mSourceTransform;
	};
	
	Character* getCharacter( const char c );
	Character* getCharacterCopy( const char c );
	void addCharacter( Character* c );
	Texture* getTexture() const { return mTexture; }
	int getSize() const { return mSize; }
	std::string getName() const { return mName; }
	bool checkDidLoad() const { return !mCharacters.empty(); }
	int defaultLineHeight;
	
private:
	std::string mName;
	int mSize;
	ci::Vec2i mTextureSize;
	Texture* mTexture;
	std::vector<Character*> mCharacters;
};