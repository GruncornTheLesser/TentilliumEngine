#pragma once
#include "Material.h"
#include "Resource.h"
#include <vector>

// interleaved - attributes in the same buffer
// seperated - attributes in seperate buffers
// interleaved vs seperate buffers
// seperate is better if single values of vertices change
// too many buffers is bad, long buffers are also bad

// direct - data is stored in an array and called upon as needed
// indexed - unique data is stored in one buffer and a different buffer stores an index pointing to the first buffer
// indexed vs direct
// indexed is better, uses less memory 
// can sometimes use fast cache


class Mesh final : public Resource<Mesh>
{
public:
	friend class Resource;
	friend class Model;
private:
	struct VBO { unsigned int index = 0, vertex = 0, tCoord = 0, normal = 0, colour = 0; } m_vbo;
	// a struct containing all the mesh data

private:
	Mesh(void* aiMsh);
public:
	Mesh(const std::vector<unsigned int>& index_data,
		const std::vector<float>& vertex_data,
		const std::vector<float>* tCoord_data = nullptr, // optional
		const std::vector<float>* normal_data = nullptr, // optional
		const std::vector<float>* colour_data = nullptr  // optional
	);
	~Mesh();

	// 
};



