#pragma once

#include "ParametricSurface.h"

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

struct UniformHandles {
    GLuint					Modelview;
    GLuint					Projection;
    GLuint					Transform;
    GLuint					NormalMatrix;
    GLuint					LightPosition;
    GLint					AmbientMaterial;
    GLint					SpecularMaterial;
    GLint					Shininess;
};

struct AttributeHandles {
    GLint					Position;
    GLint					Normal;
    GLint					DiffuseMaterial;
};

struct VboMesh {
    GLuint					VertexBuffer;
    GLuint					IndexBuffer;
    int						IndexCount;
};

class ShaderProgram {
public:
	ShaderProgram() {}
	void					use() {}
	GLuint					getProgramHandle() const { return mHandle; }
	GLuint					getAttributeHandle( std::string attribute ) { return mAttributes[ attribute ]; }
	GLuint					getUniformHandle( std::string uniform ) { return mUniforms[ uniform ]; }
private:
	std::map<std::string, GLuint> mUniforms;
	std::map<std::string, GLuint> mAttributes;
	
	GLuint					mHandle;
};

class Node {
public:
	Node() {}
	ShaderProgram			mShader;
	VboMesh					mMesh;
	ci::Vec4f					mColor;
	ci::Matrix44f					mTransform;
};

class RenderingEngine {
public:
    RenderingEngine();
    void					setup();
    void					draw( const Visual& visual ) const;
	void					createVbo( const ISurface* surface );
	
private:	
    GLuint					BuildShader(const char* source, GLenum shaderType) const;
    GLuint					BuildProgram(const char* vShader, const char* fShader) const;
	std::vector<VboMesh>	mVboMeshes;
    GLuint					m_colorRenderbuffer;
    GLuint					m_depthRenderbuffer;
	ci::CameraPersp			m_camera;
    UniformHandles			m_uniforms;
    AttributeHandles		m_attributes;
};