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

void RenderingEngine::addNode( Node* node )
{
	mNodes.push_back( node );
}

void RenderingEngine::removeNode( Node* node )
{
	
}

void RenderingEngine::addGuiNode( Gui* gui )
{
	mGuiNodes.push_back( gui->getNode() );
}

void RenderingEngine::removeGuiNode( Gui* gui )
{
	
}

VboMesh RenderingEngine::createVbo( const ObjParser* surface )
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
	if ( !texture ) return false;
	
	glGenTextures( 1, &texture->mHandle );
	glBindTexture( GL_TEXTURE_2D, texture->mHandle );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	int width = texture->getImageSize().x;
	int height = texture->getImageSize().y;
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->getImageData() );
	glGenerateMipmap( GL_TEXTURE_2D );
	
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
	glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA4, mScreenSize.x, mScreenSize.y );
	
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
	mScreenSize.x = width;
	mScreenSize.y = height;
	
	mScreenTransform = Matrix44f::identity();
	mScreenTransform.translate( Vec3f( -1.0f, 1.0f, 0.0f ) );
	mScreenTransform.scale( Vec3f( 1 / (float) width * 2.0f, 1 / (float) height * 2.0f, 1.0f ) );
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mScreenSize.x, mScreenSize.y );
    
    // Create the framebuffer object.
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
	
	m_camera = CameraPersp( width, height, 35.0f, 1.0f, 100.0f );
	m_camera.setEyePoint( Vec3f( 0, 0, 3 ) );
	m_camera.setCenterOfInterestPoint( Vec3f::zero() );
	m_camera.setWorldUp( Vec3f::yAxis() );
}

void RenderingEngine::draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Set the viewport transform.
	glViewport( 0, 0, mScreenSize.x, mScreenSize.y );
	glEnable(GL_DEPTH_TEST);
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
    
	for( auto iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		Node* node = *iter;
		
		ShaderProgram& shader = mShaders[ node->mShader ];
		
		glUseProgram( shader.getHandle() );
		shader.uniform( "AmbientMaterial",		Vec4f( 0.15f, 0.15f, 0.15f, 1.0f ) );
		shader.uniform( "LightPosition",		Vec3f( 40.0, 20.0, 10.0 ) );
		shader.uniform( "SpecularMaterial",		node->mColorSpecular );
		shader.uniform( "DiffuseMaterial",		node->mColor );
		shader.uniform( "Shininess",			node->mShininess );
		shader.uniform( "Glossiness",			node->mGlossiness );
		shader.uniform( "Modelview",			m_camera.getModelViewMatrix() );
		shader.uniform( "Projection",			m_camera.getProjectionMatrix() );
		shader.uniform( "Transform",			node->getTransform() );
		
		if ( node->mTexture != NULL ) {
			glActiveTexture( GL_TEXTURE0 );
			shader.uniform( "DiffuseTexture", 0 );
			glBindTexture( GL_TEXTURE_2D, node->mTexture->mHandle );
		}
		
		if ( node->mTextureNormal != NULL ) {
			glActiveTexture( GL_TEXTURE1 );
			shader.uniform( "NormalTexture", 1 );
			glBindTexture( GL_TEXTURE_2D, node->mTextureNormal->mHandle );
		}
		
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
		
		if ( node->mTexture != NULL ) {
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
	}
	
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	
	glEnable( GL_BLEND );
	
    
	for( auto iter = mGuiNodes.begin(); iter != mGuiNodes.end(); iter++ ) {
		Node* node = *iter;
		
		ShaderProgram& shader = mShaders[ node->mShader ];
		
		glUseProgram( shader.getHandle() );
		shader.uniform( "DiffuseMaterial",		node->mColor );
		shader.uniform( "Transform",			node->getTransform() );
		shader.uniform( "ScreenTransform",		mScreenTransform );
		shader.uniform( "ScreenSize",			mScreenSize );
		
		if ( node->mTexture != NULL ) {
			glActiveTexture( GL_TEXTURE0 );
			shader.uniform( "DiffuseTexture", 0 );
			glBindTexture( GL_TEXTURE_2D, node->mTexture->mHandle );
		}
		
		glBindBuffer( GL_ARRAY_BUFFER, node->mMesh.VertexBuffer );
		glEnableVertexAttribArray( kPositionAttribLoc );
		glVertexAttribPointer( kPositionAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
		
		glBindBuffer( GL_ARRAY_BUFFER, node->mMesh.TexCoordBuffer );
		glEnableVertexAttribArray( kTexCoordAttribLoc );
		glVertexAttribPointer( kTexCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), 0 );
		
		glDrawArrays( GL_TRIANGLES, 0, node->mMesh.VertexCount );
		
		if ( node->mTexture != NULL ) {
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
	}
}




