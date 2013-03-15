#include "RenderingEngine.h"

const static int kNormalAttribLoc		= 0;
const static int kPositionAttribLoc		= 1;
const static int kTexCoordAttribLoc		= 2;
const static int kColorAttribLoc		= 3;

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

bool RenderingEngine::createTexture( Texture* texture )
{
	glGenTextures( 1, &texture->mHandle );
	
	
	// 2 x 2 Image, 3 bytes per pixel(R, G, B)
	
	glBindTexture( GL_TEXTURE_2D, texture->mHandle );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	int width = texture->getImageSize().x;
	int height = texture->getImageSize().y;
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->getImageData() );
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//ci:Vec2i size = texture->getImageSize();
	//glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->getImageData() );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	// Unbind texture
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	// When the time comes:
	//glDeleteTextures( 1, &texture->mHandle );
	
	// TODO: Check errors more ways than this:
	return texture->mHandle != 0;
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
	m_camera.setEyePoint( Vec3f( 0, 0, 5 ) );
	m_camera.setCenterOfInterestPoint( Vec3f::zero() );
	m_camera.setWorldUp( Vec3f::yAxis() );
}

void RenderingEngine::addNode( Node* node )
{
	mNodes.push_back( node );
}

void RenderingEngine::removeNode( Node* node )
{
	
}

void RenderingEngine::draw()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Set the viewport transform.
	glViewport( 0, 0, m_windowSize.x, m_windowSize.y );
    
	for( std::list<Node*>::iterator iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		
		ShaderProgram& shader = mShaders[ node->mShader ];
		
		glUseProgram( shader.getHandle() );
		shader.uniform( "AmbientMaterial",		Vec4f( 0.25f, 0.25f, 0.25f, 1.0f ) );
		shader.uniform( "LightPosition",		Vec3f( 40.0, 20.0, 10.0 ) );
		shader.uniform( "SpecularMaterial",		node->mSpecularColor );
		shader.uniform( "DiffuseMaterial",		node->mColor );
		shader.uniform( "Shininess",			node->mShininess );
		shader.uniform( "Modelview",			m_camera.getModelViewMatrix() );
		shader.uniform( "Projection",			m_camera.getProjectionMatrix() );
		shader.uniform( "Transform",			node->mTransform );
		
		if ( node->mTexture != NULL ) {
			glActiveTexture( GL_TEXTURE0 );
			shader.uniform( "DiffuseTexture", 0 );
			glBindTexture( GL_TEXTURE_2D, node->mTexture->mHandle );
		}
		
		GLint colorLoc			= shader.getAttribLocation( "DiffuseMaterial" );
		GLint positionLoc		= shader.getAttribLocation( "Position" );
		GLint normalLoc			= shader.getAttribLocation( "Normal" );
		GLint texCoordLoc		= shader.getAttribLocation( "TextureCoord" );
		
		
		glEnable(GL_DEPTH_TEST);
		
		glBindBuffer( GL_ARRAY_BUFFER, node->mMesh.VertexBuffer );
		glEnableVertexAttribArray( kPositionAttribLoc );
		glVertexAttribPointer( kPositionAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
		
		glBindBuffer( GL_ARRAY_BUFFER, node->mMesh.NormalBuffer );
		glEnableVertexAttribArray( kNormalAttribLoc );
		glVertexAttribPointer( kNormalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
		
		glBindBuffer( GL_ARRAY_BUFFER, node->mMesh.TexCoordBuffer );
		glEnableVertexAttribArray( kTexCoordAttribLoc );
		glVertexAttribPointer( kTexCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), 0 );
		
		glDrawArrays( GL_TRIANGLES, 0, node->mMesh.VertexCount );
	}	
}




