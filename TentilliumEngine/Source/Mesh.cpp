#include "Mesh.h"
#include "OpenGL.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

std::map<std::string, gl::Buffer<GL_ARRAY_BUFFER, gl::Vertex>> s_vbo_cache;
std::map<std::tuple<unsigned int, int>, gl::Buffer<GL_ELEMENT_ARRAY_BUFFER, unsigned int>> s_ibo_cache;

void load_obj(std::string filepath)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texels;

	std::vector<gl::Vertex> vertexdata;
	std::vector<unsigned int> indicedata;

	std::ifstream stream(filepath, std::ios::in);
	std::string line;

	std::map<std::tuple<int, int, int>, int> ver_cache;

	while (getline(stream, line))
	{
		std::string type;
		std::istringstream iss(line);

		iss >> type;

		if (type.front() == '#') continue;

		if (type == "v")
		{
			float x, y, z;
			iss >> x >> y >> z;
			positions.push_back(glm::vec3(x, y, z));
		}
		else if (type == "vn")
		{
			float x, y, z;
			iss >> x >> y >> z;
			normals.push_back(glm::vec3(x, y, z));
		}
		else if (type == "vt")
		{
			float x, y;
			iss >> x >> y;
			texels.push_back(glm::vec2(x, y));
		}
		else if (type == "f")
		{
			int ip, in, it;
			char del; // delineator 

			for (int i = 0; i < 3; i++)
			{
				iss >> ip >> del >> it >> del >> in;

				auto key = std::tuple(ip, it, in);
				if (ver_cache.find(key) == ver_cache.end())
				{
					ver_cache[key] = vertexdata.size();
					indicedata.push_back(vertexdata.size());
					vertexdata.push_back(gl::Vertex(positions[ip - 1], normals[in - 1], texels[it - 1]));
				}
				else
				{
					indicedata.push_back(ver_cache[key]);
				}
				continue;
			}
		}
		// objects not currently supported
		// materials not currently supported
	}

	s_vbo_cache[filepath] = gl::Buffer<GL_ARRAY_BUFFER, gl::Vertex>(vertexdata);			// the vertex buffer object
	std::tuple ibo_key(s_vbo_cache[filepath].m_handle, GL_TRIANGLES);						// buffer key to get the ibo	
	s_ibo_cache[ibo_key] = gl::Buffer<GL_ELEMENT_ARRAY_BUFFER, unsigned int>(indicedata);	// the index buffer object

}

void generate_primitive(unsigned int vbo_handle, int Primitive)
{
	std::vector<unsigned int> tri_ibo = s_ibo_cache[std::tuple(vbo_handle, GL_TRIANGLES)].Read(); // triangle vbo
	std::vector<unsigned int> new_ibo;

	switch (Primitive)
	{
	case GL_TRIANGLES_ADJACENCY:
		new_ibo = std::vector<unsigned int>(tri_ibo.size() * 2);
		// TODO: Finish adjacency generation - needed for volume shadows
		break;
	default:
		throw std::exception("Unrecognised/Unsupported primitive");
	};

	s_ibo_cache[std::tuple(vbo_handle, Primitive)] = gl::Buffer<GL_ELEMENT_ARRAY_BUFFER, unsigned int>(new_ibo);
}

Mesh::Mesh(const char* filepath, int primitive) : m_primitive(primitive)
{
	if (s_vbo_cache.find(filepath) == s_vbo_cache.end())
		load_obj(filepath);
	m_vbo = &s_vbo_cache[filepath];

	auto ibo_key = std::tuple(m_vbo->m_handle, primitive);
	if (s_ibo_cache.find(ibo_key) == s_ibo_cache.end())
		generate_primitive(m_vbo->m_handle, primitive);
	m_ibo = &s_ibo_cache[ibo_key];

	glGenVertexArrays(1, &m_vao);
	
	glBindVertexArray(m_vao);
	m_ibo->Bind();
	m_vbo->Bind();

	int offset = 0;
	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gl::Vertex), (void*)(offset));
	offset += sizeof(glm::vec3);

	glEnableVertexAttribArray(1); // normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(gl::Vertex), (void*)(offset));
	offset += sizeof(glm::vec3);

	glEnableVertexAttribArray(2); // texel
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(gl::Vertex), (void*)(offset));
	offset += sizeof(glm::vec2);

	glBindVertexArray(0);
	m_ibo->UnBind();
	m_vbo->UnBind();

}

void Mesh::Draw()
{
	glBindVertexArray(m_vao);
	glDrawElements(m_primitive, m_ibo->m_length, GL_UNSIGNED_INT, nullptr);

}
