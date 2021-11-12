#pragma once
#include "OpenGL.h"
#include <json.h>
#include <array>
#include <map>
#include <sstream>
#include <fstream>
#include <optional>

enum VertAttrib
{
	POSITION =		0x00000001,	// Vertex position - vec3
	NORMAL =		0x00000002,	// Vertex normal - vec3
	TEXCOORD_0 =	0x00000004,	// Vertex texture coordinate - vec2
	TEXCOORD_1 =	0x00000008,	// Vertex texture coordinate - vec2
	TEXCOORD_2 =	0x00000010,	// Vertex texture coordinate - vec2
	TEXCOORD_3 =	0x00000020,	// Vertex texture coordinate - vec2
	INDICES_0 =		0x00000040,	// Vertex blend weights between 4 Indices - vec4
	WEIGHTS_0 =		0x00000080, // Vertex blend Indices for the 4 weights - ivec4
};

class Model
{
private:
	std::vector<Mesh> meshes;

public:
	Model();

};



class Mesh
{
public:
	struct Default_Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};
private:
	unsigned int VAO, VBO, IBO;
	int primitive;
	int length;
public:
	template<class ... attributes>
	Mesh(int primitive, std::vector<std::tuple<attributes...>> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> textures);
	
	void Draw();

private:
	template<class attribute>
	void AddAttribute(const int stride, int& i, int& offset);
};

template<class ... attributes>
inline Mesh::Mesh(int primitive, std::vector<std::tuple<attributes...>> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> textures)
	: primitive(primitive), length(indices.size())
{
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	int offset = 0;
	int attrib = 0;
	int stride = (sizeof...(attributes));

	(AddAttribute<attributes>(stride, &attrib, &offset), ...);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template<class attribute>
inline void Mesh::AddAttribute(const int stride, int& i, int& offset)
{
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, sizeof(attribute) / sizeof(float), GL_FLOAT, GL_FALSE, stride, (void*)(offset));
	offset += sizeof(attribute);
	i++;
}

void Mesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(primitive, length, GL_UNSIGNED_INT, nullptr);
}


/*
// vertex attributes which can be 'OR'ed together to describe a vertex
enum VertAttrib 
{
	POSITION =		0x00000001,	// Vertex position - vec3
	NORMAL =		0x00000002,	// Vertex normal - vec3
	TEXCOORD_0 =	0x00000004,	// Vertex texture coordinate - vec2
	TEXCOORD_1 =	0x00000008,	// Vertex texture coordinate - vec2
	TEXCOORD_2 =	0x00000010,	// Vertex texture coordinate - vec2
	TEXCOORD_3 =	0x00000020,	// Vertex texture coordinate - vec2
	TEXCOORD_4 =	0x00000030,	// Vertex texture coordinate - vec2
	TEXCOORD_5 =	0x00000080,	// Vertex texture coordinate - vec2
	TEXCOORD_6 =	0x00000100,	// Vertex texture coordinate - vec2
	TEXCOORD_7 =	0x00000200,	// Vertex texture coordinate - vec2
	INDICES_0 =		0x00000400,	// Vertex blend weights between 4 Indices - vec4
	WEIGHTS_0 =		0x00000800, // Vertex blend Indices for the 4 weights - ivec4
};

namespace _internal
{
	using namespace glm;

	using  vbo_key_t = const char*; 
	struct ibo_key_t { const char* filepath; int primitive; };
	struct vao_key_t { const char* filepath; int primitive;  int vertAttrib; };

	struct vbo_data_t { unsigned int handle; unsigned int length; int vertexAttrib; unsigned int count; };
	struct ibo_data_t { unsigned int handle; unsigned int length; unsigned int count; };
	struct vao_data_t { unsigned int handle; unsigned int count; };

	static std::map<vbo_key_t, vbo_data_t> s_vbo_cache;
	static std::map<vao_key_t, vao_data_t> s_vao_cache;
	static std::map<ibo_key_t, ibo_data_t> s_ibo_cache;
}

//primitive and vert attrib is in the template not because it changes the implementation of mesh but because it changes how its used
template<int primitive, int attribID>
class Mesh
{
public:
	Mesh(const char* filepath);
	Mesh();
};

template<int primitive, int attribID>
inline Mesh<primitive, attribID>::Mesh() :  m_length(0) { }

template<int primitive, int attribID>
inline Mesh<primitive, attribID>::Mesh(const char* filepath)
{
	_internal::vao_key_t vaokey{ filepath, primitive, attribID };
	_internal::ibo_key_t iboKey{ filepath, primitive };

	if (_internal::s_vao_cache.find(vaokey) != _internal::s_vao_cache.end())
	{
		m_handle = _internal::s_vao_cache[vaokey].handle;
		m_length = _internal::s_vao_cache[vaokey].length;

		_internal::s_vbo_cache[filepath].count++;
		_internal::s_ibo_cache[iboKey].count++;
		_internal::s_vao_cache[vaokey].count++;
	}
	else
	{
		_internal::ibo_key_t iboKey{ filepath, primitive };
		if (_internal::s_ibo_cache.find(iboKey) == _internal::s_ibo_cache.end())
		{
			unsigned int ibo_handle;
			glGenBuffers(1, &ibo_handle);

			unsigned int ibo_length = 0;	// temporary 
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_handle);

			// TODO: create ibo from file
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);			// unbind just so mistakes arent made
			
			_internal::s_ibo_cache[iboKey] = _internal::ibo_data_t{ ibo_handle, ibo_length, 1};
		}

		if (_internal::s_vbo_cache.find(filepath) == _internal::s_vbo_cache.end())
		{
			
			unsigned int vbo_handle;
			glGenBuffers(1, &vbo_handle);

			unsigned int vbo_length = 0;	// temporary 
			glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);

			// TODO: create vbo from file

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);			// unbind just so mistakes arent made
			
			_internal::s_ibo_cache[filepath] = _internal::vbo_data_t{ vbo_handle, vbo_length, 1 };
		}
	}
}
*/

/*{
	std::vector<vec3> positions;	// vec4
	std::vector<vec3> normals;		// vec4
	std::vector<vec2> texels;		// vec2

	//std::vector<vec4> blendWeight;	// things i want obj to support but it doesnt
	//std::vector<ivec4> blendIndice;

	std::vector<Vertex>			vertexdata;
	std::vector<unsigned int>	indicedata;

	std::ifstream stream(filepath, std::ios::in);
	std::string line;

	// tuples are convenient to use because they already have the hashing keys and all that jazz
	std::map<std::tuple<int, int, int>, int> ver_cache;

	while (getline(stream, line))
	{
		std::string type;
		std::istringstream iss(line);

		iss >> type;

		if (type.front() == '#') continue;	// comments

		if (type == "v")					// vertex position
		{
			if (vertAttrib & VertAttrib::POSITION != VertAttrib::POSITION) continue;	// if vert attribute not in mesh

			float x, y, z;
			iss >> x >> y >> z;
			positions.push_back(vec3(x, y, z));
		}
		else if (type == "vn") 				// vertex normal
		{
			if (vertAttrib & VertAttrib::NORMAL != VertAttrib::NORMAL) continue;		// if vert attribute not in mesh

			float x, y, z;
			iss >> x >> y >> z;
			normals.push_back(vec3(x, y, z));
		}
		else if (type == "vt") 				// vertex texcoord
		{
			float x, y;
			iss >> x >> y;
			texels.push_back(vec2(x, y));
		}
		else if (type == "f") 				// face
		{
			int ip, in, it;
			char del; // delineator 

			for (int i = 0; i < 3; i++)
			{
				iss >> ip >> del >> it >> del >> in;

				// this assumes all vertices contain the same vertex attributes, which isnt in the obj specification... but like?

				// lookup to check if vertex already in array
				auto facekey = std::tuple(ip, it, in);
				if (ver_cache.find(facekey) != ver_cache.end())
					indicedata.push_back(ver_cache[facekey]);
				else
				{
					// .obj uses indexing 1 -> n instead of the normal 0 -> n - 1, hence '[ip - 1]'
					vertexdata.push_back(Vertex{ positions[ip - 1], normals[in - 1], texels[it - 1], vec4(0), ivec4(0) });
					ver_cache[facekey] = vertexdata.size() - 1;	 // add look up for new vertex to the end of the array
					indicedata.push_back(vertexdata.size() - 1); // add index to the end of the array to element array
				}
				continue;
			}
		}
		// loading objects not currently supported -> could be extended to support bones
		// loading materials not currently supported
	}
}*/


