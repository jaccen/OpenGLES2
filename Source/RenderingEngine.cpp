#include "RenderingEngine.h"

#define STRINGIFY(A)  #A
#include "../Resources/Shaders/vertex_lighting.vert"
#include "../Resources/Shaders/vertex_lighting.frag"
//#include "../Resources/Shaders/pixel_lighting.vert"
//#include "../Resources/Shaders/pixel_lighting.frag"

using namespace ci;

RenderingEngine::RenderingEngine()
{
    glGenRenderbuffers( 1, &m_colorRenderbuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, m_colorRenderbuffer );
}

VboMesh RenderingEngine::createVbo( const ISurface* surface )
{
	// output
	VboMesh vboData2;
	
	// Create the VBO for the vertices.
	std::vector<float> vertices;
	surface->GenerateVertices(vertices, VertexFlagsNormals);
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	
	return vboData2;
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
	
	// Create a new VBO for the indices if needed.
	GLuint indexBuffer;
	std::vector<GLushort> indices;
	surface->GenerateTriangleIndices(indices);
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	VboMesh vboData = { vertexBuffer, indexBuffer, indices.size() };
	return vboData;
}

void RenderingEngine::createFbo()
{
	// Create renderbuffer object
	GLuint renderbuffer;
	glGenRenderbuffers( 1, &renderbuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, renderbuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA4, m_windowSize.x, m_windowSize.y );
	
	// Create framebuffer object
	GLuint colorFramebuffer;
	glGenFramebuffers( 1, &colorFramebuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, colorFramebuffer );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorFramebuffer );
}

void RenderingEngine::setup( int width, int height )
{
	m_windowSize.x = width;
	m_windowSize.y = height;
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_windowSize.x, m_windowSize.y );
    
    // Create the framebuffer object.
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
	
    // Create the GLSL program.
	mDefaultShader = ShaderProgram( SimpleVertexShader, SimpleFragmentShader );
	
	m_camera = CameraPersp( width, height, 35.0f, 1.0f, 100.0f );
	m_camera.setEyePoint( Vec3f( 0, 0, 15 ) );
	m_camera.setCenterOfInterestPoint( Vec3f::zero() );
	m_camera.setWorldUp( Vec3f::yAxis() );
}

void RenderingEngine::draw( const Node& node )
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Set the viewport transform.
	glViewport( 0, 0, m_windowSize.x, m_windowSize.y );
    
	glUseProgram( mDefaultShader.getHandle() );
	
	mDefaultShader.uniform( "Ambient",				Vec3f( 0.04f, 0.04f, 0.04f ) );
	mDefaultShader.uniform( "SpecularMaterial",		Vec3f( 0.5, 0.5, 0.5 ) );
	mDefaultShader.uniform( "Shininess",			50.f );
	mDefaultShader.uniform( "LightPosition",		Vec3f( 0.0, 20.0, 0 ) );
	mDefaultShader.uniform( "Modelview",			m_camera.getModelViewMatrix() );
	mDefaultShader.uniform( "Projection",			m_camera.getProjectionMatrix() );
	mDefaultShader.uniform( "Transform",			node.mTransform );
	
	GLint colorLoc			= mDefaultShader.getAttribLocation( "DiffuseMaterial" );
	GLint positionLoc		= mDefaultShader.getAttribLocation( "Position" );
	GLint normalLoc			= mDefaultShader.getAttribLocation( "Normal" );
	//GLint texCoordLoc		= mDefaultShader.getAttribLocation( "TextureCoord" );
	
    glEnableVertexAttribArray( positionLoc );
    glEnableVertexAttribArray( normalLoc );
    //glEnableVertexAttribArray( texCoordLoc );
	
    glEnable(GL_DEPTH_TEST);
	
	// Set the diffuse color.
	Vec4f color = node.mColor;
	glVertexAttrib4f( colorLoc, color.x, color.y, color.z, color.w );
	
	// Set vertex, normal and texcoord attributes
	int stride = 2 * sizeof(Vec3f);
	const GLvoid* offsetNormal = (const GLvoid*) sizeof(Vec3f);
	//const GLvoid* offsetTexCoord = (const GLvoid*) (sizeof(Vec3f) + sizeof(Vec2f) );
	const VboMesh& vboData = node.mMesh;
	glBindBuffer( GL_ARRAY_BUFFER, vboData.VertexBuffer );
	glVertexAttribPointer( positionLoc, 3, GL_FLOAT, GL_FALSE, stride, 0 );
	glVertexAttribPointer( normalLoc, 3, GL_FLOAT, GL_FALSE, stride, offsetNormal );
	//glVertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, stride, offsetTexCoord );
	
	// Draw
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboData.IndexBuffer );
	glDrawElements( GL_TRIANGLES, vboData.IndexCount, GL_UNSIGNED_SHORT, 0 );
}




