#include "RenderingEngine.h"


const static int kNormalAttribLoc		= 0;
const static int kPositionAttribLoc		= 1;
const static int kColorAttribLoc		= 2;
const static int kTexCoordAttribLoc		= 3;


using namespace ci;

RenderingEngine::RenderingEngine()
{
    glGenRenderbuffers( 1, &m_colorRenderbuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, m_colorRenderbuffer );
}

VboMesh RenderingEngine::createVbo( const ObjSurface* surface )
{
	// Create the VBO for the vertices.
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	std::vector<float> vertices;
	surface->GenerateVertices( vertices );
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
	
	GLuint texCoordBuffer;
	glGenBuffers(1, &texCoordBuffer);
	std::vector<float> texCoords;
	surface->GenerateTexCoords( texCoords );
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer );
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
	
	GLuint normalBuffer;
	std::vector<float> normals;
	surface->GenerateNormals( normals );
	glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer ) ;
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW );
	
	VboMesh vboData = {
		vertexBuffer,		vertices.size() / 3,
		texCoordBuffer,		texCoords.size() / 2,
		normalBuffer,		normals.size() / 3
	};
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

void RenderingEngine::addShader( std::string key, const char* vShader, const char* fShader )
{
	mShaders[ key ] = ShaderProgram( vShader, fShader );
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
    
	ShaderProgram& shader = mShaders[ node.mShader ];
	
	glUseProgram( shader.getHandle() );
	shader.uniform( "AmbientMaterial",		Vec4f( 0.25f, 0.25f, 0.25f, 1.0f ) );
	shader.uniform( "LightPosition",		Vec3f( 0.0, 20.0, 5.0 ) );
	shader.uniform( "SpecularMaterial",		node.mSpecularColor );
	shader.uniform( "DiffuseMaterial",		node.mColor );
	shader.uniform( "Shininess",			node.mShininess );
	shader.uniform( "Modelview",			m_camera.getModelViewMatrix() );
	shader.uniform( "Projection",			m_camera.getProjectionMatrix() );
	shader.uniform( "Transform",			node.mTransform );
	
	//GLint colorLoc		= shader.getAttribLocation( "DiffuseMaterial" );
	//GLint positionLoc		= shader.getAttribLocation( "Position" );
	//GLint normalLoc		= shader.getAttribLocation( "Normal" );
	//GLint texCoordLoc		= shader.getAttribLocation( "TextureCoord" );
	
    glEnable(GL_DEPTH_TEST);
	
	int strideVec3f = sizeof(Vec3f);
	
	glBindBuffer( GL_ARRAY_BUFFER, node.mMesh.VertexBuffer );
    glEnableVertexAttribArray( kPositionAttribLoc );
	glVertexAttribPointer( kPositionAttribLoc, 3, GL_FLOAT, GL_FALSE, strideVec3f, 0 );
	
	glBindBuffer( GL_ARRAY_BUFFER, node.mMesh.NormalBuffer );
    glEnableVertexAttribArray( kNormalAttribLoc );
	glVertexAttribPointer( kNormalAttribLoc, 3, GL_FLOAT, GL_FALSE, strideVec3f, 0 );
	
	glDrawArrays( GL_TRIANGLES, 0, node.mMesh.VertexCount );
}




