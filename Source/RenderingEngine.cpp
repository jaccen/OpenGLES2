#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "GameConstants.h"

using namespace ci;

void Light::update(const float deltaTime)
{
	if ( mLensFlare.canDraw() ) {
		mLensFlare.setLightPosition( mNode.getGlobalPosition() );
		mLensFlare.update();
	}
	mNode.update( deltaTime );
}

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
	
	mSortTimer = ly::Timer( boost::bind( &RenderingEngine::sortSprites, this, boost::arg<1>() ), 1.0f / 15.0f, 0 );
	mSortTimer.start();
}

void RenderingEngine::addSpriteNode( Node* node )
{
	//node->mMaterial.setTexture( "DepthMapTexture", mDepthMapFbo->mTexture );
	mSpriteNodes.push_back( node );
}

void RenderingEngine::addNode( Node* node )
{
	mObjectNodes.push_back( node );
}

void RenderingEngine::addBackgroundNode( Node* node )
{
	mBackgroundNodes.push_back( node );
}

void RenderingEngine::removeNode( Node* node, bool andCleanUp )
{
	{
		auto match = std::find( mObjectNodes.begin(), mObjectNodes.end(), node );
		if ( match != mObjectNodes.end() ) {
			if ( andCleanUp ) {
				delete *match;
			}
			mObjectNodes.erase( match );
			return;
		}
	}{
		auto match = std::find( mSpriteNodes.begin(), mSpriteNodes.end(), node );
		if ( match != mSpriteNodes.end() ) {
			if ( andCleanUp ) {
				delete *match;
			}
			mSpriteNodes.erase( match );
			return;
		}
	}{
		auto match = std::find( mBackgroundNodes.begin(), mBackgroundNodes.end(), node );
		if ( match != mBackgroundNodes.end() ) {
			if ( andCleanUp ) {
				delete *match;
			}
			mBackgroundNodes.erase( match );
			return;
		}
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

void RenderingEngine::addLight( Light* light )
{
	mLights.push_back( light );
}

void RenderingEngine::setSkyboxNode( Node* node )
{
	mSkyboxNode = node;
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
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
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
	glGenRenderbuffers( 1, &fbo->mDepthRenderbuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, fbo->mDepthRenderbuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fbo->mWidth, fbo->mHeight );
	
	// Create framebuffer object
	glGenFramebuffers( 1, &fbo->mHandle  );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo->mHandle  );
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->mTexture->mHandle, 0 );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->mDepthRenderbuffer );
	
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
	
	ci::Vec2i size = Vec2i( 1024, 1024 ) * getContentScaleFactor();
	Texture* texture = new Texture( size.x, size.y );
	texture->mFormat = GL_RGB;
	createTexture( texture );
	mMainFbo = new FramebufferObject( texture );
	createFbo( mMainFbo );
	
	texture = new Texture( size.x, size.y );
	texture->mFormat = GL_RGBA;
	createTexture( texture );
	mDepthMapFbo = new FramebufferObject( texture );
	createFbo( mDepthMapFbo );
}

void RenderingEngine::sortSprites( const float deltaTime )
{
	std::sort( mSpriteNodes.begin(), mSpriteNodes.end(), Node::sortByDistanceFromCamera );
}

void RenderingEngine::update( const float deltaTime )
{
	if ( mSkyboxNode ) {
		mSkyboxNode->position = mCamera->getGlobalPosition();
		mSkyboxNode->rotation.x = 180.0f;
		mSkyboxNode->update();
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
	
	for( auto iter = mLights.begin(); iter != mLights.end(); iter++ ) {
		(*iter)->update( deltaTime );
	}
	
	mSortTimer.update( deltaTime );
}

void RenderingEngine::bindFrameBufferObject( FramebufferObject* fbo )
{
	glBindFramebuffer( GL_FRAMEBUFFER, fbo->mHandle );
	glBindRenderbuffer( GL_RENDERBUFFER, fbo->mColorRenderbuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, fbo->mDepthRenderbuffer );
	
	glViewport( 0, 0, mScreenSize.x, mScreenSize.y );
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void RenderingEngine::bindWindowContext()
{
	glBindFramebuffer( GL_FRAMEBUFFER, mContextFramebuffer );
    glBindRenderbuffer(GL_RENDERBUFFER, mContextDepthRenderbuffer );
    glBindRenderbuffer(GL_RENDERBUFFER, mContextColorRenderbuffer );
	
	glViewport( 0, 0, mScreenSize.x, mScreenSize.y );
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

bool RenderingEngine::setUniforms( const Material& material, ShaderProgram* shader )
{
	bool texturesWereBound = false;
	int locationIndex = 0;
	
	// Loop through map of textures and set uniforms
	for( std::map<std::string, Texture*>::const_iterator t_iter = material.mTextures.begin(); t_iter != material.mTextures.end(); t_iter++ ) {
		Texture* texture = t_iter->second;
		const std::string& locationName = t_iter->first;
		glActiveTexture( GL_TEXTURE0 + locationIndex );
		if ( texture ) {
			glBindTexture( GL_TEXTURE_2D, texture->mHandle );
		}
		else {
			glBindTexture( GL_TEXTURE_2D, mDefaultWhite->mHandle );
		}
		shader->uniform( locationName, locationIndex );
		texturesWereBound = true;
		locationIndex++;
	}
	
	// Loop through map of colors and set uniforms
	for( std::map<std::string, ColorA>::const_iterator c_iter = material.mColors.begin(); c_iter != material.mColors.end(); c_iter++ ) {
		const ColorA& color = c_iter->second;
		const std::string& locationName = c_iter->first;
		shader->uniform( locationName, color );
	}
	// Loop through map of other properties and set uniforms
	for( auto prop : material.mProperties ) {
		const std::string& locationName = prop.first;
		shader->uniform( locationName, prop.second );
	}
	for( auto prop : material.mPropertiesVec2 ) {
		const std::string& locationName = prop.first;
		shader->uniform( locationName, prop.second );
	}
	for( auto prop : material.mPropertiesVec3 ) {
		const std::string& locationName = prop.first;
		shader->uniform( locationName, prop.second );
	}
	
	return texturesWereBound;
}

void RenderingEngine::unbindTextures( Material& material )
{
	int locationIndex = 0;
	for( std::map<std::string, Texture*>::const_iterator iter = material.mTextures.begin(); iter != material.mTextures.end(); iter++ ) {
		glActiveTexture( GL_TEXTURE0 + locationIndex );
		glBindTexture( GL_TEXTURE_2D, 0 );
		locationIndex++;
	}
}

void RenderingEngine::draw()
{
	/*bindFrameBufferObject( mDepthMapFbo );
	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glDisable( GL_BLEND );
	ShaderProgram* shader = ResourceManager::get()->getShader( "depth_map" );
	glUseProgram( shader->getHandle() );
	const float restoreNear = mCamera->getNearPlaneDistance();
	const float restoreFar = mCamera->getFarPlaneDistance();
	mCamera->setRange( 1.0f, 750.0f );
	shader->uniform( "Modelview", mCamera->getModelViewMatrix() );
	shader->uniform( "Projection", mCamera->getProjectionMatrix() );
	shader->uniform( "FarPlane", mCamera->getFarPlaneDistance() );
	for( auto node : mObjectNodes ) {
		shader->uniform( "Transform", node->getTransform() );
		drawMesh( node->mMesh, shader );
	}
	for( auto node : mSpriteNodes ) {
		shader->uniform( "Transform", node->getTransform() );
		drawMesh( node->mMesh, shader );
	}
	mCamera->setRange( restoreNear, restoreFar );*/
	
	bindFrameBufferObject( mMainFbo );
	
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glDisable( GL_DEPTH_TEST );
	//const float restoreNear = mCamera->getNearPlaneDistance();
	//const float restoreFar = mCamera->getFarPlaneDistance();
	//mCamera->setRange( 500.0f, 20000.0f );
	if ( mSkyboxNode ) {
		drawNode( mSkyboxNode );
	}
	else if ( mBackgroundTexture ) {
		Matrix44f trans = Matrix44f::identity();
		trans.scale( Vec3f( mBackgroundTexture->mWidth, mBackgroundTexture->mHeight, 1 ) );
		drawTexture( mBackgroundTexture, trans, true );
	}
	
	glClear( GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
    
	for( auto bgNode : mBackgroundNodes ) {
		drawNode( bgNode );
	}
	//mCamera->setRange( restoreNear, restoreFar );
    
	for( auto objNode : mObjectNodes ) {
		drawNode( objNode );
	}
	
	//glDisable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
    
	for( auto sprNode : mSpriteNodes ) {
		drawNode( sprNode );
	}
	
	bindWindowContext();
	
	// Draw the main FBO's texture
	Matrix44f trans = Matrix44f::identity();
	trans.translate( Vec3f( mMainFbo->mTexture->mWidth / (2 * getContentScaleFactor()), mMainFbo->mTexture->mHeight / (2 * getContentScaleFactor()), 0.0f ) );
	trans.scale( Vec3f( mMainFbo->mTexture->mWidth / getContentScaleFactor(), mMainFbo->mTexture->mHeight / getContentScaleFactor(), 0.0f ) );
	drawTexture( mMainFbo->mTexture, trans, true );
	
	glDisable( GL_DEPTH_TEST );
	for( auto node2d : mScreenNodes ) {
		drawGui( node2d );
	}
}

void RenderingEngine::drawNode( const Node* node )
{
	ShaderProgram* shader = node->mMaterial.mShader;
	if ( shader == NULL ) return;
	
	glUseProgram( shader->getHandle() );
	shader->uniform( "Modelview",			mCamera->getModelViewMatrix() );
	shader->uniform( "Projection",			mCamera->getProjectionMatrix() );
	shader->uniform( "EyePosition",			Vec3f( mCamera->getGlobalPosition() ) );
	
	if ( node->mFaceCamera || node->mFaceCameraAsLine ) {
		shader->uniform( "Transform", node->getTransform() * node->mSpriteTransform );
	}
	else {
		shader->uniform( "Transform", node->getTransform() );
	}
	
	for( auto light : mLights ) {
		shader->uniform( "AmbientMaterial",		light->mAmbientColor );
		shader->uniform( "LightPosition",		light->mNode.getGlobalPosition() );
		shader->uniform( "LightColor",			light->mColor );
	}
	
	setUniforms( node->getMaterial(), shader );
	
	setBlendMode( node->mLayer );
	drawMesh( node->getMesh(), node->getMaterial().mShader );
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glUseProgram( 0 );
}

void RenderingEngine::drawMesh( const Mesh* mesh, const ShaderProgram* shader, const bool wireframe )
{
	if ( mesh == NULL ) return;
	
	{
		GLint loc = glGetAttribLocation( shader->getHandle(), "Position" );
		glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
		glEnableVertexAttribArray( loc );
		glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
	}
	
	if ( mesh->normalBuffer > 0 ) {
		GLint loc = glGetAttribLocation( shader->getHandle(), "Normal" );
		glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
		glEnableVertexAttribArray( loc );
		glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), 0 );
	}
	
	if ( mesh->texCoordBuffer > 0 ) {
		GLint loc = glGetAttribLocation( shader->getHandle(), "TextureCoord" );
		glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
		glEnableVertexAttribArray( loc );
		glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), 0 );
	}
	
	glDrawArrays( wireframe ? GL_LINES : GL_TRIANGLES, 0, mesh->vertexCount );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void RenderingEngine::drawText( Text* text )
{
	/*ShaderProgram& shader = mShaders[ kShaderText ];
	
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
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );*/
}

void RenderingEngine::drawGui( Node2d* gui )
{
	if ( !gui->getIsVisible() ) return;
	
	for( auto iter = gui->getChildren().begin(); iter != gui->getChildren().end(); iter++ ) {
		drawGui( *iter );
	}
	
	Node* node = gui->getNode();
	
	ShaderProgram* shader = node->mMaterial.mShader;
	
	glUseProgram( shader->getHandle() );
	shader->uniform( "ScreenTransform",		mScreenTransform );
	shader->uniform( "Transform",			gui->getNode()->getTransform() );
	
	setUniforms( node->mMaterial, node->mMaterial.mShader );
	
	setBlendMode( node->mLayer );
	drawMesh( node->getMesh(), node->getMaterial().mShader );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	if ( gui->mText != NULL ) {
		drawTexture( gui->mText->getTexture(), node->getTransform(), false );
	}
	glUseProgram( 0 );
}

void RenderingEngine::drawTexture( Texture* texture, const ci::Matrix44f& transform, bool flipY )
{
	ShaderProgram* shader = ResourceManager::get()->getShader( "screen" );
	glUseProgram( shader->getHandle() );
	
	shader->uniform( "DiffuseMaterial",		Vec4f(1,1,1,1) );
	shader->uniform( "Transform",			transform );
	shader->uniform( "ScreenTransform",		flipY ? mScreenTransformFlippedY : mScreenTransform );
	
	glActiveTexture( GL_TEXTURE0 );
	shader->uniform( "DiffuseTexture", 0 );
	glBindTexture( GL_TEXTURE_2D, texture->mHandle );
	
	setBlendMode( Node::LayerGui );
	drawMesh( ResourceManager::get()->getMesh( "models/quad_plane.obj" ), shader );
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glUseProgram( 0 );
}

void RenderingEngine::setBlendMode( Node::Layer layer )
{
	glEnable( GL_BLEND );
	if ( layer == Node::LayerLighting ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	}
	else if ( layer == Node::LayerGui ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else if ( layer == Node::LayerClouds ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else {
		glDisable( GL_BLEND );
	}
}

void RenderingEngine::debugDrawCube( ci::Vec3f center, ci::Vec3f size, ci::ColorA color )
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

void RenderingEngine::debugDrawStrokedRect( const ci::Rectf &rect, const ci::ColorA color, ci::Matrix44f transform )
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	
	GLfloat verts[8];
	verts[0] = rect.getX1();	verts[1] = rect.getY1();
	verts[2] = rect.getX2();	verts[3] = rect.getY1();
	verts[4] = rect.getX2();	verts[5] = rect.getY2();
	verts[6] = rect.getX1();	verts[7] = rect.getY2();
	
	ShaderProgram* shader = ResourceManager::get()->getShader( "debug" );
	
	glUseProgram( shader->getHandle() );
	shader->uniform( "Color",				color );
	shader->uniform( "Modelview",			mCamera->getModelViewMatrix() * transform );
	shader->uniform( "Projection",			mCamera->getProjectionMatrix() );
	
	GLuint positionSlot = glGetAttribLocation( shader->getHandle(), "Position" );
	glEnableVertexAttribArray( positionSlot );
	glVertexAttribPointer( positionSlot, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, &verts[0] );
	glDrawArrays( GL_LINE_LOOP, 0, 4 );
	glDisableVertexAttribArray( positionSlot );
}

void RenderingEngine::debugDrawLine( ci::Vec3f from, ci::Vec3f to, ci::ColorA color )
{
	const float vertices[] = {
		from.x, from.y, from.z, to.x, to.y, to.z
	};
	
	ShaderProgram* shader = ResourceManager::get()->getShader( "debug" );
	
	glUseProgram( shader->getHandle() );
	shader->uniform( "Color",				color );
	shader->uniform( "Modelview",			mCamera->getModelViewMatrix() );
	shader->uniform( "Projection",			mCamera->getProjectionMatrix() );
	
	GLuint positionSlot = glGetAttribLocation( shader->getHandle(), "Position" );
	glEnableVertexAttribArray( positionSlot );
	glVertexAttribPointer( positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, &vertices[0] );
	glDrawArrays( GL_LINES, 0, 2 );
	glDisableVertexAttribArray( positionSlot );
}



