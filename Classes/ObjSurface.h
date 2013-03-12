#pragma once

#include "Vector.h"
#include "ISurface.h"

#include <string>
#include <vector>

class ObjSurface : public ISurface {
public:
    ObjSurface(const std::string& name);
    int					GetVertexCount() const;
    int					GetLineIndexCount() const { return 0; }
    int					GetTriangleIndexCount() const;
    void				GenerateVertices(std::vector<float>& vertices, unsigned char flags) const;
    void				GenerateLineIndices(std::vector<unsigned short>& indices) const {}
    void				GenerateTriangleIndices(std::vector<unsigned short>& indices) const;
	
private:
	std::string			m_name;
	std::vector<ivec3>	m_faces;
    mutable size_t		m_faceCount;
    mutable size_t		m_vertexCount;
    static const int	MaxLineSize = 128;
};
