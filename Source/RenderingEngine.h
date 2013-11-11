#pragma once

#include "ShaderProgram.h"
#include "ObjParser.h"
#include "ResourceLoader.h"
#include "Node.h"
#include "Node2d.h"
#include "Text.h"
#include "GameCamera.h"
#include "LensFlare.h"
#include "ParticleSystem.h"
#include "Timer.h"
#include "Light.h"

#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>

enum VertexFlags {
    VertexFlagsNormals = 1 << 0,
    VertexFlagsTexCoords = 1 << 1,
};

class Scene;

class RenderingEngine {
public:
	static RenderingEngine* get();
    void					setup( int width, int height, float contentScaleFactor );
    void					draw( const std::list<Scene*>& scenes );
	
	void					createVbo( Mesh* vboMesh, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords );
	void					createFbo( FramebufferObject* framebufferObject );
	bool					createTexture( Texture* texture );
	void					deleteTexture( Texture* texture );
	bool					createCubeTexture( Texture* texture );
	
	void					debugDrawCube( ci::Vec3f center, ci::Vec3f size, ci::ColorA color, Scene* scene );
	void					debugDrawLine( ci::Vec3f from, ci::Vec3f to, ci::ColorA color, Scene* scene );
	void					debugDrawStrokedRect( const ci::Rectf& rect, const ci::ColorA color, Scene* scene, ci::Matrix44f = ci::Matrix44f::identity() );
	void					debugScreenDrawStrokedRect( const ci::Rectf& rect, const ci::ColorA color );
	void					drawTexture( Texture* texture, const ci::Matrix44f& transform = ci::Matrix44f::identity(), bool flipY = false );
	void					drawText( Text* text );
	
	float					getContentScaleFactor() const { return mContentScaleFactor; }
	void					bindWindowContext();
	void					bindFrameBufferObject( FramebufferObject* fbo );
	
private:
    RenderingEngine();
	static RenderingEngine* sInstance;
	
	GLuint					mContextFramebuffer;
    GLuint					mContextColorRenderbuffer;
    GLuint					mContextDepthRenderbuffer;
	
	void					drawGui( Node2d* Node2d );
	inline void				drawNode( const Node* node, Scene* scene );
	inline void				drawMesh( const Mesh* mesh, const ShaderProgram* shader, const bool wireframe = false );
	inline void				setBlendMode( Node::Layer layerType );
	inline bool				setUniforms( const Material& material, ShaderProgram* shader );
	inline void				unbindTextures( Material& material );
	
	float					mContentScaleFactor;
	ci::Matrix44f			mScreenTransform;
	ci::Matrix44f			mScreenTransformFlippedY;
	ci::Vec2i				mScreenSize;
	
	Texture*				mDefaultWhite;
	Texture*				mDefaultBlack;
	FramebufferObject*		mMainFbo;
	FramebufferObject*		mDepthMapFbo;
};