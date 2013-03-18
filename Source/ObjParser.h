#pragma once

#include "cinder/Vector.h"
#include "ISurface.h"

#include <string>
#include <vector>

class ObjParser {
public:
    ObjParser(const std::string& name);
	
    int						getVertexCount() const;
    int						getLineIndexCount() const { return 0; }
    int						getTriangleIndexCount() const;
	void					getVertices(std::vector<float>& vertices ) const;
	void					getNormals(std::vector<float>& vertices ) const;
	void					getTexCoords(std::vector<float>& vertices ) const;
	
private:
	std::vector<ci::Vec3f>	m_positions;
	std::vector<ci::Vec3f>	m_normals;
	std::vector<ci::Vec3f>	m_texcoords;
	
	std::vector<int>		m_vertex_indices;
	std::vector<int>		m_normal_indices;
	std::vector<int>		m_texcoord_indices;
	
	std::string				m_name;
    mutable size_t			m_faceCount;
    mutable size_t			m_vertexCount;
    static const int		MaxLineSize = 128;
};
