#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <map>
#include <iostream>
#include "ModelComponent.h"

using namespace glm;

ModelComponent::Mesh::Mesh(unsigned int VAO, unsigned int VBO, unsigned int IBO, unsigned int length, int primitive)
	: VAO(VAO), VBO(VBO), IBO(IBO), length(length), primitive(primitive)
{ }

ModelComponent::Mesh::~Mesh()
{
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void ModelComponent::Mesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(primitive, length, GL_UNSIGNED_INT, nullptr);
}


ModelComponent::ModelComponent() 
{ 
	/*initiate an empty? idk -> do someting intelligent*/
}

ModelComponent::ModelComponent(const char* filepath)
{
	struct Vertex { vec3 position; vec3 normal; vec2 texel; };

	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<vec2> texels;
	
	std::vector<Vertex>	vertexdata;
	std::vector<GLuint>	indicedata;

	std::ifstream stream(filepath, std::ios::in);
	std::string line;

	std::map<std::array<int, 3>, int> ver_cache;

	while (getline(stream, line))
	{
		std::string type;
		std::istringstream iss(line);

		iss >> type;

		if (type.front() == '#') continue;	// comments

		if (type == "v")					// vertex position
		{
			if (false) continue;			// if use position

			float x, y, z;
			iss >> x >> y >> z;
			positions.push_back(vec3(x, y, z));
		}
		else if (type == "vn") 			// vertex normal
		{
			if (false) continue;		// if use normal

			float x, y, z;
			iss >> x >> y >> z;
			normals.push_back(vec3(x, y, z));
		}
		else if (type == "vt") 			// vertex texcoord
		{
			if (false) continue;		// if use texel

			float x, y;
			iss >> x >> y;
			texels.push_back(vec2(x, y));
		}
		else if (type == "f") 			// face
		{
			int ip, in, it;
			char del;

			for (int i = 0; i < 3; i++)
			{
				iss >> ip >> del >> it >> del >> in;

				std::array<int, 3> vert_key = { ip, it, in };		// lookup to check if vertex already in array

				if (ver_cache.find(vert_key) != ver_cache.end())	// if cache doesnt have vertex
					indicedata.push_back(ver_cache[vert_key]);
				else
				{
					vertexdata.push_back(Vertex{ positions[ip - 1], normals[in - 1], texels[it - 1] });
					ver_cache[vert_key] = vertexdata.size() - 1;	// add look up for new vertex to the end of the array
					indicedata.push_back(vertexdata.size() - 1);	// add index to the end of the array to element array
				}
			}
		}
		else if (type == "mtllib")	// load in new mtl file
		{
			std::string mtlfile;
			iss >> mtlfile;
			std::cout << mtlfile << "\n";
		}

		else if (type == "usemtl")	// dictate which file to use for the next mesh definition
		{
			std::string mtl;
			iss >> mtl;
			std::cout << mtl << "\n";

		}
		// loading objects not currently supported -> could be extended to support bones
		// loading materials not currently supported
	}

}

ModelComponent::~ModelComponent()
{
	/*destroy model -> do something intelligent*/
}

void ModelComponent::Draw()
{
	for (const auto& mesh : m_meshes)
		mesh->Draw();
}

ModelComponent::Material::Material(const char* filepath)
{
	std::ifstream stream(filepath, std::ios::in);
	std::string line;

	while (getline(stream, line))
	{
		std::string identifier;
		std::istringstream iss(line);

		iss >> identifier;

		if (identifier.front() == '#') continue;	// comments

		if (identifier == "newmtl") continue;		// begin new material

		else if (identifier == "Ka") continue;		// ambient 

		else if (identifier == "Ks") continue;		// specular

		else if (identifier == "Kd") continue;		// diffuse

		else if (identifier == "map_Ka") continue;	// ambient texture

		else if (identifier == "map_Ks") continue;	// specular texture

		else if (identifier == "map_Kd") continue;	// diffuse texture

		else if (identifier == "d") continue;

		else if (identifier == "Ns") continue;		// specular exponent

	}
}
