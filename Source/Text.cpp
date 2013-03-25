#include "Text.h"
#include "ResourceManager.h"
#include "ResourceLoader.h"

#include "cinder/Vector.h"

#include <iostream>

using namespace ci;

Text::Text()
{
}

Text::~Text() {}

Texture* Text::drawToTexture()
{
	std::string textString = "G";
	int numChars = textString.size();
	const char* textChars = textString.c_str();
	int letterSpacing = 1;
	int fontSize = 10;
	int width = 800;
	
	RenderingEngine* renderer = RenderingEngine::get();
	
	FT_Library library;
	FT_Error error = FT_Init_FreeType( &library );
	if ( error ) {
		std::cout << "Error initializing freetype library." << std::endl;
		return NULL;
	}
	
	FT_Face face;
	std::string relativePath = "fonts/Menlo.ttc";
	std::string path = mResourceLoader.getResourcePath() + "/" + relativePath;
	error = FT_New_Face( library, path.c_str(), 0, &face );
	if ( error == FT_Err_Unknown_File_Format ) {
		std::cout << "Error loading font: Unknown format." << std::endl;
		return NULL;
	}
	else if ( error ) {
		std::cout << "Error loading font." << std::endl;
		return NULL;
	}
	else {
		std::cout << "Font loaded: " << relativePath << std::endl;
		std::cout << "\t - Num Faces: " << face->num_faces << std::endl;
	}
	
	Vec2i size = Camera::get()->getScaledScreenSize();
	error = FT_Set_Char_Size( face, 0, fontSize*64, size.x, size.x );
	if ( error ) {
		std::cout << "Error setting charater size" << std::endl;
		return NULL;
	}
	
	Texture* stringTexture = new Texture( 1024, 512 );
	renderer->createTexture( stringTexture );
	FramebufferObject* fbo = new FramebufferObject( stringTexture );
	renderer->createFbo( fbo );
	renderer->bindFrameBufferObject( fbo );
	
	int offsetX = 0;
	
	for( int c = 0; c < numChars; c++ ) {
		int glyph_index = FT_Get_Char_Index( face, textChars[ c ] );
		if ( glyph_index == 0 ) {
			std::cout << "Error getting glyph index." << std::endl;
			return NULL;
		}
		
		int32_t flags = FT_LOAD_DEFAULT;
		//flags << FT_LOAD_NO_BITMAP;
		error = FT_Load_Glyph( face, glyph_index, flags );
		if ( error ) {
			std::cout << "Erroring loading glyph: " << glyph_index << std::endl;
			return NULL;
		}
		else {
			std::cout << "Glyph loaded, format: ";
			switch( face->glyph->format ) {
				case FT_GLYPH_FORMAT_NONE:
					std::cout << "FT_GLYPH_FORMAT_NONE";
					break;
				case FT_GLYPH_FORMAT_COMPOSITE:
					std::cout << "FT_GLYPH_FORMAT_COMPOSITE";
					break;
				case FT_GLYPH_FORMAT_BITMAP:
					std::cout << "FT_GLYPH_FORMAT_BITMAP";
					break;
				case FT_GLYPH_FORMAT_OUTLINE:
					std::cout << "FT_GLYPH_FORMAT_OUTLINE";
					break;
				case FT_GLYPH_FORMAT_PLOTTER:
					std::cout << "FT_GLYPH_FORMAT_PLOTTER";
					break;
			}
			std::cout << std::endl;
		}
		
		if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP ) {
			std::cout << "Rendering bitmap...";
			FT_Render_Mode render_mode = FT_RENDER_MODE_NORMAL;
			error = FT_Render_Glyph( face->glyph, render_mode );
			if ( error ) {
				std::cout << "Error" << std::endl;
			}
			else {
				std::cout << "Rendered '" << textChars[c] << "':" << std::endl;
				std::cout << "\t - width: " << face->glyph->bitmap.width << std::endl;
				std::cout << "\t - rows: " << face->glyph->bitmap.rows << std::endl;
				std::cout << "\t - left: " << face->glyph->bitmap_left << std::endl;
				std::cout << "\t - top: " << face->glyph->bitmap_top << std::endl;
			}
		}
		
		Texture* texture = new Texture();
		texture->mFormat = GL_ALPHA;
		texture->setSize( face->glyph->bitmap.width, face->glyph->bitmap.rows );
		
		GLubyte data[ texture->mWidth * texture->mHeight ];
		int d = 0;
		for( int y = 0; y < texture->mHeight; y++ ) {
			for( int x = 0; x < texture->mWidth; x++ ) {
				int i = y * texture->mWidth + x;
				//std::cout << "i: " << i << std::endl;
				if ( y >= face->glyph->bitmap.rows ) {
					data[i] = 0;
				}
				else if ( x >= face->glyph->bitmap.width ) {
					data[i] = 0;
				}
				else {
					data[i] = face->glyph->bitmap.buffer[ d++ ];
				}
			}
		}
		
		texture->mImageData = data;
		renderer->createTexture( texture );
		renderer->drawTexture( texture, offsetX, -texture->mHeight + face->glyph->bitmap_top );
		//renderer->deleteTexture( texture );
		//delete texture;
		
		offsetX += face->glyph->bitmap.width + letterSpacing * fontSize;
	}
	//renderer->deleteFbo( fbo );
	//delete fbo;
	return stringTexture;
}



