#pragma once

#include "ParametricSurface.h"
#include "ShaderProgram.h"
#include "ObjSurface.h"
#include "ResourceManager.h"

#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/Camera.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>

enum VertexFlags {
    VertexFlagsNormals = 1 << 0,
    VertexFlagsTexCoords = 1 << 1,
};

struct VboMesh {
    GLuint					VertexBuffer;
	int						VertexCount;
    GLuint					TexCoordBuffer;
    int						TexCoordCount;
    GLuint					NormalBuffer;
    int						NormalCount;
    GLuint					IndexBuffer;
    int						IndexCount;
};

class Node {
public:
	Node() : mTexture(NULL) {}
	
	std::string				mShader;
	VboMesh					mMesh;
	ci::Vec4f				mColor;
	ci::Vec4f				mSpecularColor;
	ci::Matrix44f			mTransform;
	float					mShininess;
	Texture*				mTexture;
};

class RenderingEngine {
public:
    RenderingEngine();
    void					setup( int width, int height );
    void					draw();
	VboMesh					createVbo( const ObjSurface* surface );
	void					createFbo();
	bool					createTexture( Texture* texture );
	void					addNode( Node* node );
	void					removeNode( Node* node );
	void					addShader( std::string key, const char* vShader, const char* fShader );
	
private:
	std::list<Node*>		mNodes;
	std::map<std::string, ShaderProgram> mShaders;
	ci::Vec2i				m_windowSize;
    GLuint					m_colorRenderbuffer;
    GLuint					m_depthRenderbuffer;
	ci::CameraPersp			m_camera;
};