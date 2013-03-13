#pragma once

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "cinder/Matrix.h"
#include "cinder/Vector.h"

#include <map>
#include <string>

class ShaderProgram {
public:
	ShaderProgram() : mHandle(0) {}
	ShaderProgram( const char* vShader, const char* fShader );
	ShaderProgram( std::string vShaderPath, std::string fShaderPath );
	virtual ~ShaderProgram();
	
	GLuint					getHandle() const { return mHandle; }
	GLint					getUniformLocation( const std::string &name );
	GLint					getAttribLocation( const std::string &name );
	
	void					uniform( const std::string &name, int data );
	void					uniform( const std::string &name, const ci::Vec2i &data );
	void					uniform( const std::string &name, const int *data, int count );
	void					uniform( const std::string &name, const ci::Vec2i *data, int count );
	void					uniform( const std::string &name, float data );
	void					uniform( const std::string &name, const ci::Vec2f &data );
	void					uniform( const std::string &name, const ci::Vec3f &data );
	void					uniform( const std::string &name, const ci::Vec4f &data );
	void					uniform( const std::string &name, const ci::Matrix33f &data, bool transpose = false );
	void					uniform( const std::string &name, const ci::Matrix44f &data, bool transpose = false );
	void					uniform( const std::string &name, const float *data, int count );
	void					uniform( const std::string &name, const ci::Vec2f *data, int count );
	void					uniform( const std::string &name, const ci::Vec3f *data, int count );
	void					uniform( const std::string &name, const ci::Vec4f *data, int count );
	
	
private:
	GLuint					mHandle;
	
	std::map<std::string, GLuint> mUniforms;
	std::map<std::string, GLuint> mAttributes;
	
	GLuint					buildShader(const char* source, GLenum shaderType) const;
    GLuint					buildProgram(const char* vShader, const char* fShader) const;
	
};