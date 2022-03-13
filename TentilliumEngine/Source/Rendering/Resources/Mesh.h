#pragma once
#include "Resource.h"
#include "Material.h"
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

struct Mesh : Resource<Mesh>
{
	friend class Resource;
	friend class Model;
private:
	struct VBO { unsigned int index = 0, vertex = 0, tCoord = 0, normal = 0, colour = 0; };
	std::shared_ptr<Material> m_mat;	// a material is stored along side the mesh
	VBO m_vbo;							// a struct containing all the mesh data

private:
	Mesh(void* aiMsh, std::shared_ptr<Material> material);
public:
	Mesh(const std::shared_ptr<Material> material,
		const std::vector<unsigned int>& index_data,
		const std::vector<float>& vertex_data,
		const std::vector<float>* tCoord_data = nullptr, // optional
		const std::vector<float>* normal_data = nullptr, // optional
		const std::vector<float>* colour_data = nullptr  // optional
	);
	~Mesh();

	// 
};



