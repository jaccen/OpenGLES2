#pragma once

#include "ParametricSurface.h"
#include "ShaderProgram.h"
#include "ObjSurface.h"

#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/Camera.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>

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
	Node() {}
	std::string				mShader;
	VboMesh					mMesh;
	ci::Vec4f				mColor;
	ci::Vec4f				mSpecularColor;
	ci::Matrix44f			mTransform;
	float					mShininess;
};

class RenderingEngine {
public:
    RenderingEngine();
    void					setup( int width, int height );
    void					draw( const Node& visual );
	VboMesh					createVbo( const ObjSurface* surface );
	void					createFbo();
	void					addShader( std::string key, const char* vShader, const char* fShader );
	
private:
	std::map<std::string, ShaderProgram> mShaders;
	ci::Vec2i				m_windowSize;
    GLuint					m_colorRenderbuffer;
    GLuint					m_depthRenderbuffer;
	ci::CameraPersp			m_camera;
};