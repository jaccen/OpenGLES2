#pragma once

#include "ParametricSurface.h"
#include "ShaderProgram.h"

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

struct Visual {
    ci::Vec3f Color;
    ci::Vec2i LowerLeft;
    ci::Vec2i ViewportSize;
	ci::Matrix44f Transform;
};

struct VboMesh {
    GLuint					VertexBuffer;
    GLuint					IndexBuffer;
    int						IndexCount;
};

class Node {
public:
	Node() {}
	ShaderProgram			mShader;
	VboMesh					mMesh;
	ci::Vec4f				mColor;
	ci::Matrix44f			mTransform;
};

class RenderingEngine {
public:
    RenderingEngine();
    void					setup( int width, int height );
    void					draw( const Visual& visual );
	void					createVbo( const ISurface* surface );
	void					createFbo();
	
private:
	ShaderProgram			mDefaultShader;
	ci::Vec2i				m_windowSize;
	std::vector<VboMesh>	mVboMeshes;
    GLuint					m_colorRenderbuffer;
    GLuint					m_depthRenderbuffer;
	ci::CameraPersp			m_camera;
};