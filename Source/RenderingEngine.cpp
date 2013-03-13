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

void RenderingEngine::createVbo( const ISurface* surface )
{
	// Create the VBO for the vertices.
	std::vector<float> vertices;
	surface->GenerateVertices(vertices, VertexFlagsNormals);
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
	
	// Create a new VBO for the indices if needed.
	int indexCount = surface->GetTriangleIndexCount();
	GLuint indexBuffer;
	std::vector<GLushort> indices(indexCount);
	surface->GenerateTriangleIndices(indices);
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	VboMesh vboData = { vertexBuffer, indexBuffer, indexCount };
	mVboMeshes.push_back( vboData );
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
	glUseProgram( mDefaultShader.getHandle() );

    // Initialize various state.
    glEnableVertexAttribArray( mDefaultShader.getAttribLocation( "Position" ) );
    glEnableVertexAttribArray( mDefaultShader.getAttribLocation( "Normal" ) );
    glEnable(GL_DEPTH_TEST);
	
	m_camera = CameraPersp( width, height, 35.0f, 1.0f, 100.0f );
	m_camera.setEyePoint( Vec3f( 0, 0, 15 ) );
	m_camera.setCenterOfInterestPoint( Vec3f::zero() );
	m_camera.setWorldUp( Vec3f::yAxis() );
}

void RenderingEngine::draw( const Visual& visual )
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Set the viewport transform.
	Vec2i size = visual.ViewportSize;
	Vec2i lowerLeft = visual.LowerLeft;
	glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);
	
	mDefaultShader.uniform( "Ambient", Vec3f( 0.04f, 0.04f, 0.04f ) );
	mDefaultShader.uniform( "SpecularMaterial", Vec3f( 0.5, 0.5, 0.5 ) );
	mDefaultShader.uniform( "Shininess", 50.f );
	mDefaultShader.uniform( "LightPosition", Vec3f( 0.0, 20.0, 0 ) );
	mDefaultShader.uniform( "Modelview", m_camera.getModelViewMatrix() );
	mDefaultShader.uniform( "Projection", m_camera.getProjectionMatrix() );
	mDefaultShader.uniform( "Transform", visual.Transform );
	
	// Set the diffuse color.
	GLint colorLoc = mDefaultShader.getAttribLocation( "DiffuseMaterial" );
	GLint positionLoc = mDefaultShader.getAttribLocation( "Position" );
	GLint normalLoc = mDefaultShader.getAttribLocation( "Normal" );
	
	Vec3f color = visual.Color * 0.75f;
	glVertexAttrib4f( colorLoc, color.x, color.y, color.z, 1);
	
	// Draw the surface.
	int stride = 2 * sizeof(Vec3f);
	const GLvoid* offset = (const GLvoid*) sizeof(Vec3f);
	const VboMesh& vboData = mVboMeshes[0];
	
	glBindBuffer( GL_ARRAY_BUFFER, vboData.VertexBuffer );
	glVertexAttribPointer( positionLoc, 3, GL_FLOAT, GL_FALSE, stride, 0 );
	glVertexAttribPointer( normalLoc, 3, GL_FLOAT, GL_FALSE, stride, offset );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboData.IndexBuffer );
	glDrawElements( GL_TRIANGLES, vboData.IndexCount, GL_UNSIGNED_SHORT, 0 );
}
