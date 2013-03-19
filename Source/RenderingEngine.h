#pragma once

#include "ShaderProgram.h"
#include "ObjParser.h"
#include "ResourceLoader.h"
#include "Node.h"
#include "Node2d.h"
#include "Camera.h"

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

class RenderingEngine {
public:
    RenderingEngine();
    void					setup( int width, int height, float contentScaleFactor );
	void					update( const float deltaTime = 0.0f );
    void					draw();
	void					createVbo( Mesh* vboMesh, std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texCoords );
	void					createFbo();
	bool					createTexture( Texture* texture );
	void					setRootGui( Node2d* Node2d );
	void					setSkyboxNode( Node* node );
	void					addNode( Node* node );
	void					removeNode( Node* node );
	void					addNode( Node2d* node );
	void					removeNode( Node2d* node );
	void					addShader( std::string key, const char* vShader, const char* fShader );
	
	void					debugDrawCube( ci::Vec3f center, ci::Vec3f size, ci::Vec4f color );
	void					debugDrawLine( ci::Vec3f from, ci::Vec3f to, ci::Vec4f color );
	
private:
	void					drawGui( Node2d* Node2d );
	void					drawMesh( Mesh* mesh, bool wireframe = false );
	void					setBlendMode( Node::Layer layerType );
	
	float					mContentScaleFactor;
	ci::Matrix44f			mScreenTransform;
	ci::Vec2i				mScreenSize;
	std::list<Node*>		mObjectNodes;
	std::list<Node2d*>		mScreenNodes;
	std::map<std::string, ShaderProgram> mShaders;
    GLuint					m_colorRenderbuffer;
    GLuint					m_depthRenderbuffer;
	Camera*					mCamera;
	Node*					mSkyboxNode;
	Node2d*					mRootGui;
};