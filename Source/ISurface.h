#pragma once

#include <vector>

struct ISurface {
    virtual int			GetVertexCount() const = 0;
    virtual int			GetLineIndexCount() const = 0;
    virtual int			GetTriangleIndexCount() const = 0;
    virtual void		GenerateVertices( std::vector<float>& vertices, unsigned char flags = 0 ) const = 0;
    virtual void		GenerateLineIndices( std::vector<unsigned short>& indices ) const = 0;
    virtual void		GenerateTriangleIndices( std::vector<unsigned short>& indices ) const = 0;
    virtual ~ISurface() {}
};