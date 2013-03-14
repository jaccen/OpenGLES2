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
	{
		// Normals
		ifstream objFile(m_name.c_str());
		while (objFile) {
			char c[3];
			objFile.get( c, 3 );
			if ( c[0] == 'v' && c[1] != 'n' && c[1] != 't' ) {
				Vec3f position;
				objFile >> position.x >> position.y >> position.z;
				m_positions.push_back( position );
			}
			else if ( c[0] == 'v' && c[1] == 'n' ) {
				Vec3f normal;
				objFile >> normal.x >> normal.y >> normal.z;
				m_normals.push_back( normal );
			}
			else if ( c[0] == 'v' && c[1] == 't' ) {
				Vec3f texcoord;
				objFile >> texcoord.x >> texcoord.y >> texcoord.z;
				m_texcoords.push_back( texcoord );
			}
			else if (c[0] == 'f') {
				m_faceCount++;
				
				for( int i = 0; i < 3; i++ ) {
					int v_index;
					objFile >> v_index;
					m_vertex_indices.push_back( v_index );
					objFile.ignore( MaxLineSize, '/' );
					
					int t_index;
					objFile >> t_index;
					m_texcoord_indices.push_back( t_index );
					objFile.ignore( MaxLineSize, '/' );
					
					int n_index;
					objFile >> n_index;
					m_normal_indices.push_back( n_index );
				}
			}
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

void ObjSurface::GenerateVertices(vector<float>& floats ) const
{
	int size = m_faceCount * 3 * 3;
	floats.resize( size );
	int v = 0;
	for( int i = 0; i < size; i+=3 ) {
		int vi = m_vertex_indices[ v ]-1;
		floats[ i+0 ] = m_positions[ vi ].x;
		floats[ i+1 ] = m_positions[ vi ].y;
		floats[ i+2 ] = m_positions[ vi ].z;
		v++;
	}
}

void ObjSurface::GenerateTexCoords(vector<float>& floats ) const
{
	int size = m_faceCount * 3 * 2;
	floats.resize( size );
	int v = 0;
	for( int i = 0; i < size; i+=2 ) {
		int vi = m_texcoord_indices[ v ]-1;
		floats[ i+0 ] = m_texcoords[ vi ].x;
		floats[ i+1 ] = m_texcoords[ vi ].y;
		v++;
	}
}

void ObjSurface::GenerateNormals(vector<float>& floats ) const
{
	int size = m_faceCount * 3 * 3;
	floats.resize( size );
	int v = 0;
	for( int i = 0; i < size; i+=3 ) {
		int vi = m_normal_indices[ v ]-1;
		floats[ i+0 ] = m_normals[ vi ].x;
		floats[ i+1 ] = m_normals[ vi ].y;
		floats[ i+2 ] = m_normals[ vi ].z;
		v++;
	}
}

void ObjSurface::GenerateTriangleIndices(vector<unsigned short>& indices) const
{
	indices.resize( m_faceCount * 3 );
    ifstream objFile(m_name.c_str());
	int index = 0;
    while (objFile) {
        char c = objFile.get();
        if ( c == 'f' ) {
			for( int i = 0; i < 3; i++ ) {
				int p_index;
				objFile >> p_index;
				std::cout << p_index << std::endl;
				indices[ index++ ] = p_index-1;
				objFile.ignore( MaxLineSize, '/' );
				objFile.ignore( MaxLineSize, ' ' );
			}
		}
		objFile.ignore(MaxLineSize, '\n');
	}
	objFile.close();
}
