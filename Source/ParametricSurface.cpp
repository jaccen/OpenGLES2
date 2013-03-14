#include "ParametricSurface.h"
#include "RenderingEngine.h"

void ParametricSurface::SetInterval(const ParametricInterval& interval)
{
    m_divisions = interval.Divisions;
    m_upperBound = interval.UpperBound;
    m_textureCount = interval.TextureCount;
    m_slices = m_divisions - ci::Vec2i(1, 1);
}

int ParametricSurface::GetVertexCount() const
{
    return m_divisions.x * m_divisions.y;
}

int ParametricSurface::GetLineIndexCount() const
{
    return 4 * m_slices.x * m_slices.x;
}

int ParametricSurface::GetTriangleIndexCount() const
{
    return 6 * m_slices.x * m_slices.y;
}

ci::Vec2f ParametricSurface::ComputeDomain(float x, float y) const
{
    return ci::Vec2f(x * m_upperBound.x / m_slices.x, y * m_upperBound.y / m_slices.y);
}

void ParametricSurface::GenerateVertices(std::vector<float>& vertices, unsigned char flags) const
{
    int floatsPerVertex = 3;
    if (flags & VertexFlagsNormals)
        floatsPerVertex += 3;
    if (flags & VertexFlagsTexCoords)
        floatsPerVertex += 2;

    vertices.resize(GetVertexCount() * floatsPerVertex);

	int k = 0;
    for (int j = 0; j < m_divisions.y; j++) {
        for (int i = 0; i < m_divisions.x; i++) {

            // Compute Position
            ci::Vec2f domain = ComputeDomain(i, j);
            ci::Vec3f range = Evaluate(domain);
            vertices[k+0] = range.x;
            vertices[k+1] = range.y;
            vertices[k+2] = range.z;

            // Compute Normal
            if (flags & VertexFlagsNormals) {
                float s = i, t = j;

                // Nudge the point if the normal is indeterminate.
                if (i == 0) s += 0.01f;
                if (i == m_divisions.x - 1) s -= 0.01f;
                if (j == 0) t += 0.01f;
                if (j == m_divisions.y - 1) t -= 0.01f;
                
                // Compute the tangents and their cross product.
                ci::Vec3f p = Evaluate(ComputeDomain(s, t));
                ci::Vec3f u = Evaluate(ComputeDomain(s + 0.01f, t)) - p;
                ci::Vec3f v = Evaluate(ComputeDomain(s, t + 0.01f)) - p;
                ci::Vec3f normal = u.cross(v).normalized();
                if (InvertNormal(domain))
                    normal = -normal;
				vertices[k+3] = normal.x;
				vertices[k+4] = normal.y;
				vertices[k+5] = normal.z;
            }
            
            // Compute Texture Coordinates
            if (flags & VertexFlagsTexCoords) {
                float s = m_textureCount.x * i / m_slices.x;
                float t = m_textureCount.y * j / m_slices.y;
				ci::Vec2f texCoord = ci::Vec2f( s, t );
				vertices[k+6] = texCoord.x;
				vertices[k+7] = texCoord.y;
            }
			k += floatsPerVertex;
        }
    }
}

void ParametricSurface::GenerateLineIndices(std::vector<unsigned short>& indices) const
{
    indices.resize(GetLineIndexCount());
    std::vector<unsigned short>::iterator index = indices.begin();
    for (int j = 0, vertex = 0; j < m_slices.y; j++) {
        for (int i = 0; i < m_slices.x; i++) {
            int next = (i + 1) % m_divisions.x;
            *index++ = vertex + i;
            *index++ = vertex + next;
            *index++ = vertex + i;
            *index++ = vertex + i + m_divisions.x;
        }
        vertex += m_divisions.x;
    }
}

void
ParametricSurface::GenerateTriangleIndices(std::vector<unsigned short>& indices) const
{
    indices.resize(GetTriangleIndexCount());
    std::vector<unsigned short>::iterator index = indices.begin();
    for (int j = 0, vertex = 0; j < m_slices.y; j++) {
        for (int i = 0; i < m_slices.x; i++) {
            int next = (i + 1) % m_divisions.x;
            *index++ = vertex + i;
            *index++ = vertex + next;
            *index++ = vertex + i + m_divisions.x;
            *index++ = vertex + next;
            *index++ = vertex + next + m_divisions.x;
            *index++ = vertex + i + m_divisions.x;
        }
        vertex += m_divisions.x;
    }
}
