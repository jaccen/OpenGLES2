#pragma once

#include "Vector.h"
#include "ISurface.h"

#include <vector>

struct ParametricInterval {
    ivec2			Divisions;
    vec2			UpperBound;
    vec2			TextureCount;
};

class ParametricSurface : ISurface {
public:
    int				GetVertexCount() const;
    int				GetLineIndexCount() const;
    int				GetTriangleIndexCount() const;
    void			GenerateVertices( std::vector<float>& vertices, unsigned char flags) const;
    void			GenerateLineIndices( std::vector<unsigned short>& indices) const;
    void			GenerateTriangleIndices( std::vector<unsigned short>& indices) const;
protected:
    void			SetInterval(const ParametricInterval& interval);
    virtual vec3	Evaluate(const vec2& domain) const = 0;
    virtual bool	InvertNormal(const vec2& domain) const { return false; }
private:
    vec2			ComputeDomain(float i, float j) const;
    ivec2			m_slices;
    ivec2			m_divisions;
    vec2			m_upperBound;
	vec2			m_textureCount;
};
