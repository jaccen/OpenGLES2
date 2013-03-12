#include "RenderingEngine.h"

#define STRINGIFY(A)  #A
#include "../Resources/Shaders/SimpleLighting.es2.vert"
#include "../Resources/Shaders/SimpleLighting.es2.frag"

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
	if (!mVboMeshes.empty() && indexCount == mVboMeshes[0].IndexCount) {
		indexBuffer = mVboMeshes[0].IndexBuffer;
	} else {
		std::vector<GLushort> indices(indexCount);
		surface->GenerateTriangleIndices(indices);
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	}
	VboMesh vboData = { vertexBuffer, indexBuffer, indexCount };
	mVboMeshes.push_back( vboData );
}

void RenderingEngine::setup()
{
    // Extract width and height.
    int width, height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    // Create a depth buffer that has the same size as the color buffer.
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    
    // Create the framebuffer object.
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
    
    // Create the GLSL program.
    GLuint program = BuildProgram(SimpleVertexShader, SimpleFragmentShader);
    glUseProgram(program);
	
	// TODO: glUseProgram( shader->getHandle() );

    // Extract the handles to attributes and uniforms.
    m_attributes.Position						= glGetAttribLocation(program, "Position");
    m_attributes.Normal							= glGetAttribLocation(program, "Normal");
    m_attributes.DiffuseMaterial				= glGetAttribLocation(program, "DiffuseMaterial");
	
    m_uniforms.Projection						= glGetUniformLocation(program, "Projection");
    m_uniforms.Modelview						= glGetUniformLocation(program, "Modelview");
    m_uniforms.Transform						= glGetUniformLocation(program, "Transform");
    m_uniforms.NormalMatrix						= glGetUniformLocation(program, "NormalMatrix");
    m_uniforms.LightPosition					= glGetUniformLocation(program, "LightPosition");
    m_uniforms.AmbientMaterial					= glGetUniformLocation(program, "AmbientMaterial");
    m_uniforms.SpecularMaterial					= glGetUniformLocation(program, "SpecularMaterial");
    m_uniforms.Shininess						= glGetUniformLocation(program, "Shininess"); 
    
    // Set up some default material parameters.
    glUniform3f( m_uniforms.AmbientMaterial, 0.04f, 0.04f, 0.04f);
    glUniform3f( m_uniforms.SpecularMaterial, 0.5, 0.5, 0.5);
    glUniform1f( m_uniforms.Shininess, 50);

    // Initialize various state.
    glEnableVertexAttribArray(m_attributes.Position);
    glEnableVertexAttribArray(m_attributes.Normal);
    glEnable(GL_DEPTH_TEST);
	
	m_camera = ci::CameraPersp( width, height, 35.0f, 1.0f, 100.0f );
	m_camera.setEyePoint( ci::Vec3f( 0, 0, 15 ) );
	m_camera.setCenterOfInterestPoint( ci::Vec3f::zero() );
	m_camera.setWorldUp( ci::Vec3f::yAxis() );
}

void RenderingEngine::draw( const Visual& visual ) const
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Set the viewport transform.
	ci::Vec2i size = visual.ViewportSize;
	ci::Vec2i lowerLeft = visual.LowerLeft;
	glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);
	
	// Set the light position.
	ci::Vec4f lightPosition(0.0, 20.0, 0, 0);
	glUniform3fv(m_uniforms.LightPosition, 1, lightPosition.ptr() );
	
	// Set modelview matrix
	glUniformMatrix4fv(m_uniforms.Modelview, 1, 0, m_camera.getModelViewMatrix().m );
	
	// Set projection matrix
	glUniformMatrix4fv( m_uniforms.Projection, 1, 0, m_camera.getProjectionMatrix().m );
	
	// Set modelview matrix
	glUniformMatrix4fv(m_uniforms.Transform, 1, 0, visual.Transform.m );
	
	// Set the normal matrix.
	// It's orthogonal, so its Inverse-Transpose is itself!
	ci::Matrix33f normalMatrix = ci::Matrix33f::identity();
	glUniformMatrix3fv(m_uniforms.NormalMatrix, 1, 0, normalMatrix.m );
	
	// Set the diffuse color.
	ci::Vec3f color = visual.Color * 0.75f;
	glVertexAttrib4f(m_attributes.DiffuseMaterial, color.x, color.y, color.z, 1);
	
	// Draw the surface.
	int stride = 2 * sizeof(ci::Vec3f);
	const GLvoid* offset = (const GLvoid*) sizeof(ci::Vec3f);
	GLint position = m_attributes.Position;
	GLint normal = m_attributes.Normal;
	const VboMesh& vboData = mVboMeshes[0];
	
	glBindBuffer(GL_ARRAY_BUFFER, vboData.VertexBuffer);
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboData.IndexBuffer);
	glDrawElements(GL_TRIANGLES, vboData.IndexCount, GL_UNSIGNED_SHORT, 0);
}

GLuint RenderingEngine::BuildShader(const char* source, GLenum shaderType) const
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);
    
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages;
        exit(1);
    }
    
    return shaderHandle;
}

GLuint RenderingEngine::BuildProgram(const char* vertexShaderSource, const char* fragmentShaderSource) const
{
    GLuint vertexShader = BuildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = BuildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages;
        exit(1);
    }
    
    return programHandle;
}
