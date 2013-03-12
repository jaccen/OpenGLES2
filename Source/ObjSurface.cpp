#include "ObjSurface.h"
#include "RenderingEngine.h"

#import <list>
#import <fstream>
#import <assert.h>

using namespace std;

ObjSurface::ObjSurface(const string& name) :
    m_name(name),
    m_faceCount(0),
    m_vertexCount(0)
{
    m_faces.resize(GetTriangleIndexCount() / 3);
    ifstream objFile(m_name.c_str());
    vector<ci::Vec3i>::iterator face = m_faces.begin();
    while (objFile) {
        char c = objFile.get();
        if (c == 'f') {
            assert(face != m_faces.end() && "parse error");
            objFile >> face->x >> face->y >> face->z;
            *face++ -= ci::Vec3i(1, 1, 1);
        }
        objFile.ignore(MaxLineSize, '\n');
    }
    assert(face == m_faces.end() && "parse error");
}

int ObjSurface::GetVertexCount() const
{
    if (m_vertexCount != 0)
        return m_vertexCount;
    
    ifstream objFile(m_name.c_str());
    while (objFile) {
        char c = objFile.get();
        if (c == 'v')
            m_vertexCount++;
        objFile.ignore(MaxLineSize, '\n');
    }
    return m_vertexCount;
}

int ObjSurface::GetTriangleIndexCount() const
{
    if (m_faceCount != 0)
        return m_faceCount * 3;
    
    ifstream objFile(m_name.c_str());
    while (objFile) {
        char c = objFile.get();
        if (c == 'f')
            m_faceCount++;
        objFile.ignore(MaxLineSize, '\n');
    }
    return m_faceCount * 3;
}

void ObjSurface::GenerateVertices(vector<float>& floats, unsigned char flags) const
{
    assert(flags == VertexFlagsNormals && "Unsupported flags.");

    struct Vertex {
        ci::Vec3f Position;
        ci::Vec3f Normal;
    };

    // Read in the vertex positions and initialize lighting normals to (0, 0, 0).
    floats.resize(GetVertexCount() * 6);
    ifstream objFile(m_name.c_str());
    Vertex* vertex = (Vertex*) &floats[0];
    while (objFile) {
        char c = objFile.get();
        if (c == 'v') {
            vertex->Normal = ci::Vec3f(0, 0, 0);
            ci::Vec3f& position = (vertex++)->Position;
            objFile >> position.x >> position.y >> position.z;
        }
        objFile.ignore(MaxLineSize, '\n');
    }

    vertex = (Vertex*) &floats[0];
    for (size_t faceIndex = 0; faceIndex < m_faces.size(); ++faceIndex) {
        ci::Vec3i face = m_faces[faceIndex];

        // Compute the facet normal.
        ci::Vec3f a = vertex[face.x].Position;
        ci::Vec3f b = vertex[face.y].Position;
        ci::Vec3f c = vertex[face.z].Position;
        ci::Vec3f facetNormal = (b - a).cross(c - a);

        // Add the facet normal to the lighting normal of each adjoining vertex.
        vertex[face.x].Normal += facetNormal;
        vertex[face.y].Normal += facetNormal;
        vertex[face.z].Normal += facetNormal;
    }

    // Normalize the normals.
    for (int v = 0; v < GetVertexCount(); ++v)
        vertex[v].Normal.normalize();
}

void ObjSurface::GenerateTriangleIndices(vector<unsigned short>& indices) const
{
    indices.resize(GetTriangleIndexCount());
    vector<unsigned short>::iterator index = indices.begin();
    for (vector<ci::Vec3i>::const_iterator f = m_faces.begin(); f != m_faces.end(); ++f) {
        *index++ = f->x;
        *index++ = f->y;
        *index++ = f->z;
    }
}
