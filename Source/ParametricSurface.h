#pragma once

#include "ISurface.h"

#include "cinder/Vector.h"

#include <vector>

struct ParametricInterval {
    ci::Vec2i			Divisions;
    ci::Vec2f			UpperBound;
    ci::Vec2f			TextureCount;
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
    virtual ci::Vec3f	Evaluate(const ci::Vec2i& domain) const = 0;
    virtual bool	InvertNormal(const ci::Vec2i& domain) const { return false; }
private:
    ci::Vec2f			ComputeDomain(float i, float j) const;
    ci::Vec2f			m_slices;
    ci::Vec2i			m_divisions;
    ci::Vec2f			m_upperBound;
	ci::Vec2f			m_textureCount;
};
