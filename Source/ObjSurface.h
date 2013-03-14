#pragma once

#include "cinder/Vector.h"
#include "ISurface.h"

#include <string>
#include <vector>

class ObjSurface : public ISurface {
public:
    ObjSurface(const std::string& name);
    int					GetVertexCount() const;
    int					GetLineIndexCount() const { return 0; }
    int					GetTriangleIndexCount() const;
    virtual void		GenerateVertices(std::vector<float>& vertices, unsigned char flags) const;
    virtual void		GenerateLineIndices(std::vector<unsigned short>& indices) const {}
    virtual void		GenerateTriangleIndices(std::vector<unsigned short>& indices) const;
	
private:
	std::vector<ci::Vec3i>	m_faces;
	std::vector<ci::Vec3f>	m_positions;
	std::vector<ci::Vec3f>	m_normals;
	std::vector<ci::Vec3f>	m_texcoords;
	
	std::string			m_name;
    mutable size_t		m_faceCount;
    mutable size_t		m_vertexCount;
    static const int	MaxLineSize = 128;
};
