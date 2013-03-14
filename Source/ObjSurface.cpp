#include "ObjSurface.h"
#include "RenderingEngine.h"

#import <list>
#import <fstream>
#import <assert.h>

using namespace std;
using namespace ci;

ObjSurface::ObjSurface(const string& name) :
    m_name(name),
    m_faceCount(0),
    m_vertexCount(0)
{
    m_faces.resize( GetTriangleIndexCount() / 3 );
	
	{
		// Positions
		ifstream objFile(m_name.c_str());
		while (objFile) {
			char c[3];
			objFile.get( c, 3 );
			if ( c[0] == 'v' && c[1] != 'n' && c[1] != 't' ) {
				Vec3f position;
				objFile >> position.x >> position.y >> position.z;
				//std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
				m_positions.push_back( position );
			}
			objFile.ignore(MaxLineSize, '\n');
		}
		objFile.close();
	}{
		// Normals
		ifstream objFile(m_name.c_str());
		while (objFile) {
			char c[3];
			objFile.get( c, 3 );
			if ( c[0] == 'v' && c[1] == 'n' ) {
				Vec3f normal;
				objFile >> normal.x >> normal.y >> normal.z;
				//std::cout << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
				m_normals.push_back( normal );
			}
			objFile.ignore(MaxLineSize, '\n');
		}
		objFile.close();
	}{
		// Texture Coordinates
		ifstream objFile(m_name.c_str());
		while (objFile) {
			char c[3];
			objFile.get( c, 3 );
			if ( c[0] == 'v' && c[1] == 't' ) {
				Vec3f texcoord;
				objFile >> texcoord.x >> texcoord.y >> texcoord.z;
				//std::cout << texcoord.x << ", " << texcoord.y << ", " << texcoord.z << std::endl;
				m_texcoords.push_back( texcoord );
			}
			objFile.ignore(MaxLineSize, '\n');
		}
		objFile.close();
	}{
		// Faces
		ifstream objFile(m_name.c_str());
		while (objFile) {
			char c = objFile.get();
			if (c == 'f')
				m_faceCount++;
			objFile.ignore(MaxLineSize, '\n');
		}
		objFile.close();
	}
	
	m_vertexCount = m_positions.size() + m_normals.size() + m_texcoords.size();
}

int ObjSurface::GetVertexCount() const
{
    return m_vertexCount;
}

int ObjSurface::GetTriangleIndexCount() const
{
    return m_faceCount * 3;
}

void ObjSurface::GenerateVertices(vector<float>& floats, unsigned char flags) const
{
    assert(flags == VertexFlagsNormals && "Unsupported flags.");

    struct Vertex {
        Vec3f Position;
        Vec3f Normal;
        Vec2f TexCoord;
    };

	int floatsPerVertex = 8;
	int v_index = 0;
    floats.resize( GetVertexCount() );
    ifstream objFile(m_name.c_str());
    while (objFile) {
        char c = objFile.get();
        if ( c == 'f' ) {
			for( int i = 0; i < 3; i++ ) {
				
				Vertex* vertex = (Vertex*) &floats[ v_index ];
				
				Vec3f& p = vertex->Position;
				Vec3f& n = vertex->Normal;
				Vec2f& t = vertex->TexCoord;
				
				int p_index;
				objFile >> p_index;
				p = m_positions[ p_index-1 ];
				objFile.ignore( MaxLineSize, '/' );
				
				int n_index;
				objFile >> n_index;
				n = m_normals[ n_index-1 ];
				objFile.ignore(MaxLineSize, '/');
				
				int t_index;
				objFile >> t_index;
				Vec3f texCoord = m_texcoords[ t_index-1 ];
				t = texCoord.xy();
				objFile.ignore(MaxLineSize, '/');
				
				std::cout << p.x << ", " << p.y << ", " << p.z << std::endl;
				std::cout << n.x << ", " << n.y << ", " << n.z << std::endl;
				std::cout << t.x << ", " << t.y << std::endl;
				std::cout << std::endl;
				
				v_index += floatsPerVertex;
			}
        }
		objFile.ignore(MaxLineSize, '\n');
    }
	objFile.close();
}

void ObjSurface::GenerateTriangleIndices(vector<unsigned short>& indices) const
{
    indices.resize(GetTriangleIndexCount());
    vector<unsigned short>::iterator index = indices.begin();
    for (vector<Vec3i>::const_iterator f = m_faces.begin(); f != m_faces.end(); ++f) {
        *index++ = f->x;
        *index++ = f->y;
        *index++ = f->z;
    }
}
