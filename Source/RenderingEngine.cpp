#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "GameConstants.h"

const static int kNormalAttribLoc		= 0;
const static int kPositionAttribLoc		= 1;
const static int kTexCoordAttribLoc		= 2;
const static int kColorAttribLoc		= 3;

using namespace ci;

RenderingEngine* RenderingEngine::sInstance = NULL;

RenderingEngine* RenderingEngine::get()
{
	if ( !sInstance ) {
		sInstance = new RenderingEngine();
	}
	return sInstance;
}

RenderingEngine::RenderingEngine() : mContentScaleFactor(1.0f), mSkyboxNode(NULL), mBackgroundTexture(NULL)
{
    glGenRenderbuffers( 1, &mContextColorRenderbuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, mContextColorRenderbuffer );
}

void RenderingEngine::addSpriteNode( Node* node )
{
	auto match = std::find( mSpriteNodes.begin(), mSpriteNodes.end(), node );
	if ( match == mSpriteNodes.end() ) {
		mSpriteNodes.push_back( node );
	}
}

void RenderingEngine::addNode( Node* node )
{
	auto match = std::find( mObjectNodes.begin(), mObjectNodes.end(), node );
	if ( match == mObjectNodes.end() ) {
		mObjectNodes.push_back( node );
	}
}

void RenderingEngine::removeNode( Node* node )
{
	auto match = std::find( mObjectNodes.begin(), mObjectNodes.end(), node );
	if ( match != mObjectNodes.end() ) {
		mObjectNodes.erase( match );
	}
}

void RenderingEngine::addNode( Node2d* node )
{
	auto match = std::find( mScreenNodes.begin(), mScreenNodes.end(), node );
	if ( match == mScreenNodes.end() ) {
		mScreenNodes.push_back( node );
	}
}

void RenderingEngine::removeNode( Node2d* node )
{
	auto match = std::find( mScreenNodes.begin(), mScreenNodes.end(), node );
	if ( match != mScreenNodes.end() ) {
		mScreenNodes.erase( match );
	}
}

void RenderingEngine::setSkyboxNode( Node* node )
{
	mSkyboxNode = node;
	mSkyboxNode->update();
}

void RenderingEngine::setBackgroundTexture( Texture* texture )
{
	mBackgroundTexture = texture;
}

void RenderingEngine::createVbo( Mesh* vboMesh, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords )
{
	// Create the VBO for the vertices.
	glGenBuffers(1, &vboMesh->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vboMesh->vertexBuffer );
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
	vboMesh->vertexCount = vertices.size() / 3;
	
	glGenBuffers(1, &vboMesh->texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vboMesh->texCoordBuffer );
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
	vboMesh->texCoordCount = texCoords.size() / 2;
	
	glGenBuffers(1, &vboMesh->normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vboMesh->normalBuffer ) ;
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW );
	vboMesh->normalCount = normals.size() / 3;
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
	glTexImage2D( GL_TEXTURE_2D, 0, texture->mFormat, texture->mWidth, texture->mHeight, 0, texture->mFormat, GL_UNSIGNED_BYTE, texture->mImageData );
	glGenerateMipmap( GL_TEXTURE_2D );
	
	// Unbind texture
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	// TODO: Check errors more ways than this:
	return texture->mHandle != 0;
}

void RenderingEngine::deleteTexture( Texture* texture )
{
	glDeleteTextures( 1, &texture->mHandle );
}

void RenderingEngine::createFbo( FramebufferObject* fbo )
{	
	GLint maxRenderbufferSize;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
	std::cout << "maxRenderbufferSize = " << maxRenderbufferSize << std::endl;
	std::cout << "Creating FBO of size: " << fbo->mWidth << "x" << fbo->mHeight << std::endl;
	
	// Create renderbuffer object
	glGenRenderbuffers( 1, &fbo->mColorRenderbuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, fbo->mColorRenderbuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, fbo->mFormat, fbo->mWidth, fbo->mHeight );
	
	// Create framebuffer object
	glGenFramebuffers( 1, &fbo->mHandle  );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo->mHandle  );
	
	if ( fbo->mTexture ) {
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->mTexture->mHandle, 0 );
	}
	else {
		//glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorFramebuffer );
	}
	
	// Check status
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( status == GL_FRAMEBUFFER_COMPLETE ) {
		std::cout << "Framebuffer created successfully." << std::endl;
	}
	else {
		std::cout << "Error creating framebuffer: ";
		switch( status ) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
				std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS" << std::endl;
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
				break;
		}
		std::cout << std::endl;
	}
}

void RenderingEngine::addShader( std::string key, const char* vShader, const char* fShader )
{
	mShaders[ key ] = ShaderProgram( vShader, fShader );
}

void RenderingEngine::setup( int width, int height, float contentScaleFactor )
{
	mScreenSize.x = width * contentScaleFactor;
	mScreenSize.y = height * contentScaleFactor;
	mContentScaleFactor = contentScaleFactor;
	
	mCamera = Camera::get();
	mCamera->setScreenSize( width, height, contentScaleFactor );
	
	mScreenTransform = Matrix44f::identity();
	mScreenTransform.translate( Vec3f( -1.0f, 1.0f, 0.0f ) );
	mScreenTransform.scale( Vec3f( 1 / (float) width * 2.0f,
								   1 / (float) height * 2.0f * -1.0f,
								  1.0f ) );
	
	mScreenTransformFlippedY = mScreenTransform;
	mScreenTransformFlippedY.translate( Vec3f( 0, mScreenSize.y / mContentScaleFactor, 0 ) );
	mScreenTransformFlippedY.scale( Vec3f( 1, -1, 1 ) );
	
	GLubyte whitePixel[4] = { 255, 255, 255, 255 };
	mDefaultWhite = new Texture();
	mDefaultWhite->mWidth = mDefaultWhite->mHeight = 1;
	mDefaultWhite->mImageData = (void*) whitePixel;
	createTexture( mDefaultWhite );
	
	GLubyte blackPixel[4] = { 0, 0, 0, 0 };
	mDefaultBlack = new Texture();
	mDefaultBlack->mWidth = mDefaultBlack->mHeight = 1;
	mDefaultBlack->mImageData = (void*) blackPixel;
	createTexture( mDefaultBlack );
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &mContextDepthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mContextDepthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mScreenSize.x, mScreenSize.y );
    
    // Create the framebuffer object.
    glGenFramebuffers(1, &mContextFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mContextFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mContextColorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mContextDepthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mContextColorRenderbuffer);
	
	/*ci::Vec2i size = Vec2i( 1024, 1024 ) * getContentScaleFactor();
	Texture* texture = new Texture( size.x, size.y );
	createTexture( texture );
	mMainFbo = new FramebufferObject( texture );
	createFbo( mMainFbo );*/
}

void RenderingEngine::update( const float deltaTime )
{
	if ( mSkyboxNode ) {
		mSkyboxNode->position = mCamera->getGlobalPosition();
	}
	
	for( auto iter = mObjectNodes.begin(); iter != mObjectNodes.end(); iter++ ) {
		(*iter)->update( deltaTime );
	}
	
	for( auto iter = mSpriteNodes.begin(); iter != mSpriteNodes.end(); iter++ ) {
		(*iter)->update( deltaTime );
	}
	
	for( auto iter = mScreenNodes.begin(); iter != mScreenNodes.end(); iter++ ) {
		(*iter)->update( deltaTime );
	}
}

void RenderingEngine::bindFrameBufferObject( FramebufferObject* fbo )
{
	glBindFramebuffer( GL_FRAMEBUFFER, fbo->mHandle );
	glBindRenderbuffer( GL_RENDERBUFFER, fbo->mColorRenderbuffer );
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glViewport( 0, 0, mScreenSize.x, mScreenSize.y );
}

void RenderingEngine::bindWindowContext()
{
	glBindFramebuffer( GL_FRAMEBUFFER, mContextFramebuffer );
    glBindRenderbuffer(GL_RENDERBUFFER, mContextDepthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mContextColorRenderbuffer);
}

void RenderingEngine::draw()
{
	bindWindowContext();
	
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Set the viewport transform.
	glViewport( 0, 0, mScreenSize.x, mScreenSize.y );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	
	if ( mSkyboxNode ) {
		
		Node* node = mSkyboxNode;
		ShaderProgram& shader = mShaders[ node->mShader ];
		
		glUseProgram( shader.getHandle() );
		shader.uniform( "DiffuseMaterial",		node->mColor );
		shader.uniform( "Modelview",			mCamera->getModelViewMatrix() );
		shader.uniform( "Projection",			mCamera->getProjectionMatrix() );
		shader.uniform( "Transform",			node->getTransform() );
		
		if ( node->mTexture != NULL ) {
			glActiveTexture( GL_TEXTURE0 );
			shader.uniform( "DiffuseTexture", 0 );
			glBindTexture( GL_TEXTURE_2D, node->mTexture->mHandle );
		}
		
		setBlendMode( node->mLayer );
		drawMesh( node->mMesh );
		
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	
	else if ( mBackgroundTexture ) {
		
		glDisable( GL_DEPTH_TEST );
		setBlendMode( Node::LayerGui );
		Matrix44f trans = Matrix44f::identity();
		trans.scale( Vec3f( mBackgroundTexture->mWidth, mBackgroundTexture->mHeight, 1 ) );
		drawTexture( mBackgroundTexture, trans, true );
	}
	
	glEnable(GL_DEPTH_TEST);
    
	for( auto iter = mObjectNodes.begin(); iter != mObjectNodes.end(); iter++ ) {
		drawNode( *iter );
	}
    
	for( auto iter = mSpriteNodes.begin(); iter != mSpriteNodes.end(); iter++ ) {
		drawNode( *iter );
	}
	
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	
	for( auto iter = mScreenNodes.begin(); iter != mScreenNodes.end(); iter++ ) {
		drawGui( *iter );
	}
	
	/*
	// Bind back to the main context framebuffer and renderbuffers
	glBindFramebuffer( GL_FRAMEBUFFER, mContextFramebuffer );
    glBindRenderbuffer(GL_RENDERBUFFER, mContextDepthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mContextColorRenderbuffer);
	
    glClearColor(0.0f, 0.0f, 0.0f, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	float csf = getContentScaleFactor();
	Matrix44f transform = Matrix44f::identity();
	transform.translate( Vec3f( mMainFbo->mWidth / 2 / csf, mMainFbo->mHeight / 4 / csf, 0.0f ) );
	transform.scale( Vec3f( mMainFbo->mWidth / csf, -mMainFbo->mHeight / csf, 1.0f ) );
	
	ShaderProgram& shader = mShaders[ kShaderScreenSpace ];
	glUseProgram( shader.getHandle() );
	shader.uniform( "DiffuseMaterial",		Vec4f(1, 1, 1, 1 ) );
	shader.uniform( "Transform",			transform );
	shader.uniform( "ScreenTransform",		mScreenTransform );
	
	glActiveTexture( GL_TEXTURE0 );
	shader.uniform( "DiffuseTexture", 0 );
	glBindTexture( GL_TEXTURE_2D, mMainFbo->mTexture->mHandle );
	
	setBlendMode( Node::LayerGui );
	drawMesh( ResourceManager::get()->getMesh( "models/quad_plane.obj" ) );
	 */
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void RenderingEngine::drawNode( Node* node )
{
	ShaderProgram& shader = mShaders[ node->mShader ];
	
	glUseProgram( shader.getHandle() );
	shader.uniform( "AmbientMaterial",		Vec4f( 0.2f, 0.2f, 0.2f, 1.0f ) );
	//shader.uniform( "LightPosition",		Vec3f( 0, -1, 0 ) );
	//shader.uniform( "LightColor",			Vec4f( 1, 0, 0, 1 ) );
	shader.uniform( "LightPosition",		Vec3f( 100.0, 50.0, 50.0 ) );
	shader.uniform( "LightColor",			Vec4f( 1, 1, 1, 1 ) );
	shader.uniform( "EyePosition",			mCamera->getGlobalPosition() );
	shader.uniform( "SpecularMaterial",		node->mColorSpecular );
	shader.uniform( "SelfIlliminationMaterial", node->mColorSelfIllumination );
	shader.uniform( "DiffuseMaterial",		node->mColor );
	shader.uniform( "RimMaterial",			node->mColorRim );
	shader.uniform( "Shininess",			node->mShininess );
	shader.uniform( "Glossiness",			node->mGlossiness );
	shader.uniform( "RimPower",				node->mRimPower );
	shader.uniform( "Modelview",			mCamera->getModelViewMatrix() );
	shader.uniform( "Projection",			mCamera->getProjectionMatrix() );
	shader.uniform( "Transform",			node->getTransform() );
	
	glActiveTexture( GL_TEXTURE0 );
	shader.uniform( "DiffuseTexture", 0 );
	bool t = node->mTexture == NULL;
	glBindTexture( GL_TEXTURE_2D, t ? mDefaultWhite->mHandle : node->mTexture->mHandle );
	
	if ( node->mTextureNormal != NULL ) {
		glActiveTexture( GL_TEXTURE1 );
		shader.uniform( "NormalTexture", 1 );
		glBindTexture( GL_TEXTURE_2D, node->mTextureNormal->mHandle );
	}
	
	if ( node->mTextureSpecular != NULL ) {
		glActiveTexture( GL_TEXTURE2 );
		shader.uniform( "SpecularTexture", 2 );
		glBindTexture( GL_TEXTURE_2D, node->mTextureSpecular->mHandle );
	}
	
	if ( node->mTextureSelfIllumination != NULL ) {
		glActiveTexture( GL_TEXTURE3 );
		shader.uniform( "SelfIlliminationTexture", 3 );
		glBindTexture( GL_TEXTURE_2D, node->mTextureSelfIllumination->mHandle );
	}
	
	setBlendMode( node->mLayer );
	drawMesh( node->mMesh );
	
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void RenderingEngine::drawMesh( Mesh* mesh, bool wireframe )
{
	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	
	glEnableVertexAttribArray( kPositionAttribLoc );
	glVertexAttribPointer( kPositionAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
	
	if ( mesh->normalBuffer > 0 ) {
		glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
		glEnableVertexAttribArray( kNormalAttribLoc );
		glVertexAttribPointer( kNormalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
	}
	
	if ( mesh->texCoordBuffer > 0 ) {
		glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
		glEnableVertexAttribArray( kTexCoordAttribLoc );
		glVertexAttribPointer( kTexCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), 0 );
	}
	
	glDrawArrays( wireframe ? GL_LINES : GL_TRIANGLES, 0, mesh->vertexCount );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void RenderingEngine::drawText( Text* text )
{
	ShaderProgram& shader = mShaders[ kShaderText ];
	
	glDisable( GL_CULL_FACE );
	
	glUseProgram( shader.getHandle() );
	shader.uniform( "DiffuseMaterial",		text->getColor() );
	shader.uniform( "ScreenTransform",		mScreenTransformFlippedY );
	
	glActiveTexture( GL_TEXTURE0 );
	shader.uniform( "DiffuseTexture", 0 );
	glBindTexture( GL_TEXTURE_2D, text->getFont()->getTexture()->mHandle );
	
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	const std::vector<Font::Character*>& chars = text->getCharacters();
	for( std::vector<Font::Character*>::const_iterator iter = chars.begin(); iter != chars.end(); iter++ ) {
		if ( (*iter)->visible ) {
			shader.uniform( "Transform", (*iter)->getTransform() );
			shader.uniform( "SourceTransform", (*iter)->getSourceTransform() );
			drawMesh( text->getMesh() );
		}
	}
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void RenderingEngine::drawGui( Node2d* gui )
{
	if ( !gui->getIsVisible() ) return;
	
	for( auto iter = gui->getChildren().begin(); iter != gui->getChildren().end(); iter++ ) {
		drawGui( *iter );
	}
	
	Node* node = gui->getNode();
	
	ShaderProgram& shader = mShaders[ node->mShader ];
	
	glUseProgram( shader.getHandle() );
	shader.uniform( "DiffuseMaterial",		node->mColor );
	shader.uniform( "Transform",			node->getTransform() );
	shader.uniform( "ScreenTransform",		mScreenTransform );
	
	glActiveTexture( GL_TEXTURE0 );
	shader.uniform( "DiffuseTexture", 0 );
	bool t = node->mTexture == NULL;
	glBindTexture( GL_TEXTURE_2D, t ? mDefaultWhite->mHandle : node->mTexture->mHandle );
	
	setBlendMode( node->mLayer );
	drawMesh( node->mMesh );
	
	if ( node->mTexture != NULL ) {
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	if ( gui->mText != NULL ) {
		drawTexture( gui->mText->getTexture(), node->getTransform(), false );
	}
}

void RenderingEngine::drawTexture( Texture* texture, const ci::Matrix44f& transform, bool flipY )
{
	ShaderProgram& shader = mShaders[ kShaderScreenSpace ];
	glUseProgram( shader.getHandle() );
	
	shader.uniform( "DiffuseMaterial",		Vec4f(1,1,1,1) );
	shader.uniform( "Transform",			transform );
	shader.uniform( "ScreenTransform",		flipY ? mScreenTransformFlippedY : mScreenTransform );
	
	glActiveTexture( GL_TEXTURE0 );
	shader.uniform( "DiffuseTexture", 0 );
	glBindTexture( GL_TEXTURE_2D, texture->mHandle );
	
	setBlendMode( Node::LayerGui );
	drawMesh( ResourceManager::get()->getMesh( "models/quad_plane.obj" ) );
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void RenderingEngine::setBlendMode( Node::Layer layer )
{
	glEnable( GL_BLEND );
	if ( layer == Node::LayerLighting ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	}
	else if ( layer == Node::LayerGui | layer == Node::LayerObjects ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else if ( layer == Node::LayerClouds ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else {
		glDisable( GL_BLEND );
	}
}

void RenderingEngine::debugDrawCube( ci::Vec3f center, ci::Vec3f size, ci::Vec4f color )
{
	Vec3f min = center - size * 0.5f;
	Vec3f max = center + size * 0.5f;
	
	debugDrawLine( Vec3f(min.x, min.y, min.z), Vec3f(max.x, min.y, min.z), color );
	debugDrawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, max.y, min.z), color );
	debugDrawLine( Vec3f(max.x, max.y, min.z), Vec3f(min.x, max.y, min.z), color );
	debugDrawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, min.y, min.z), color );
	
	debugDrawLine( Vec3f(min.x, min.y, max.z), Vec3f(max.x, min.y, max.z), color );
	debugDrawLine( Vec3f(max.x, min.y, max.z), Vec3f(max.x, max.y, max.z), color );
	debugDrawLine( Vec3f(max.x, max.y, max.z), Vec3f(min.x, max.y, max.z), color );
	debugDrawLine( Vec3f(min.x, max.y, max.z), Vec3f(min.x, min.y, max.z), color );
	
	debugDrawLine( Vec3f(min.x, min.y, min.z), Vec3f(min.x, min.y, max.z), color );
	debugDrawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, max.y, max.z), color );
	debugDrawLine( Vec3f(max.x, max.y, min.z), Vec3f(max.x, max.y, max.z), color );
	debugDrawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, min.y, max.z), color );
}

void RenderingEngine::debugDrawLine( ci::Vec3f from, ci::Vec3f to, ci::Vec4f color )
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	
	const float vertices[] = {
		from.x, from.y, from.z, to.x, to.y, to.z
	};
	
	ShaderProgram& shader = mShaders[ kShaderDebug ];
	
	glUseProgram( shader.getHandle() );
	shader.uniform( "Color",				color );
	shader.uniform( "Modelview",			mCamera->getModelViewMatrix() );
	shader.uniform( "Projection",			mCamera->getProjectionMatrix() );

	GLuint positionSlot = glGetAttribLocation( shader.getHandle(), "Position" );
	glEnableVertexAttribArray( positionSlot );
	glVertexAttribPointer( positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, &vertices[0] );
	glDrawArrays( GL_LINES, 0, 2 );
	glDisableVertexAttribArray( positionSlot );
}



