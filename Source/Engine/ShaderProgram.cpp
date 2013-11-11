#include "ShaderProgram.h"

#include <iostream>

using namespace ci;

ShaderProgram::ShaderProgram( const char* vShader, const char* fShader ) 
{
	mHandle = buildProgram( vShader, fShader );
}

ShaderProgram::ShaderProgram( std::string vShaderPath, std::string fShaderPath )
{
	//mHandle = buildProgram( vShader, fShader );
}

ShaderProgram::~ShaderProgram() {}

GLuint ShaderProgram::buildShader(const char* source, GLenum shaderType) const
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);
    
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages << std::endl;
        exit(1);
    }
    
    return shaderHandle;
}

GLuint ShaderProgram::buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource) const
{
    GLuint vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages << std::endl;
    }
    
    return programHandle;
}

GLint ShaderProgram::getUniformLocation( const std::string &name )
{
	std::map<std::string,GLuint>::const_iterator uniformIt = mUniforms.find( name );
	if( uniformIt == mUniforms.end() ) {
		GLint loc = glGetUniformLocation( mHandle, name.c_str() );
		mUniforms[ name ] = loc;
		return loc;
	}
	else
		return uniformIt->second;
}

GLint ShaderProgram::getAttribLocation( const std::string &name )
{
	return glGetAttribLocation( mHandle, name.c_str() );
}

void ShaderProgram::uniform( const std::string &name, int data )
{
	GLint loc = getUniformLocation( name );
	glUniform1i( loc, data );
}

void ShaderProgram::uniform( const std::string &name, const Vec2i &data )
{
	GLint loc = getUniformLocation( name );
	glUniform2i( loc, data.x, data.y );
}

void ShaderProgram::uniform( const std::string &name, const int *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform1iv( loc, count, data );
}

void ShaderProgram::uniform( const std::string &name, const Vec2i *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform2iv( loc, count, &data[0].x );
}

void ShaderProgram::uniform( const std::string &name, float data )
{
	GLint loc = getUniformLocation( name );
	glUniform1f( loc, data );
}

void ShaderProgram::uniform( const std::string &name, const Vec2f &data )
{
	GLint loc = getUniformLocation( name );
	glUniform2f( loc, data.x, data.y );
}

void ShaderProgram::uniform( const std::string &name, const Vec3f &data )
{
	GLint loc = getUniformLocation( name );
	glUniform3f( loc, data.x, data.y, data.z );
}

void ShaderProgram::uniform( const std::string &name, const Vec4f &data )
{
	GLint loc = getUniformLocation( name );
	glUniform4f( loc, data.x, data.y, data.z, data.w );
}

void ShaderProgram::uniform( const std::string &name, const ci::ColorA &data )
{
	GLint loc = getUniformLocation( name );
	glUniform4f( loc, data.r, data.g, data.b, data.a );
}

void ShaderProgram::uniform( const std::string &name, const float *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform1fv( loc, count, data );
}

void ShaderProgram::uniform( const std::string &name, const Vec2f *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform2fv( loc, count, &data[0].x );
}

void ShaderProgram::uniform( const std::string &name, const Vec3f *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform3fv( loc, count, &data[0].x );
}

void ShaderProgram::uniform( const std::string &name, const Vec4f *data, int count )
{
	GLint loc = getUniformLocation( name );
	glUniform4fv( loc, count, &data[0].x );
}

void ShaderProgram::uniform( const std::string &name, const Matrix33f &data, bool transpose )
{
	GLint loc = getUniformLocation( name );
	glUniformMatrix3fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, data.m );
}

void ShaderProgram::uniform( const std::string &name, const Matrix44f &data, bool transpose )
{
	GLint loc = getUniformLocation( name );
	glUniformMatrix4fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, data.m );
}