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
	/* 
	* a container of all the vertex attribute buffers
	* if Mesh doesnt have attribute, the pointer handle == 0.
	* This way its easy to check if an attribute is used in
	* this mesh. This can then be matched to the shader so all
	* attributes are sent to the GPU transferred efficiently.
	*/
	unsigned int index_buffer = 0;
	unsigned int vertex_buffer = 0;
	unsigned int tCoord_buffer = 0;
	unsigned int normal_buffer = 0;
	unsigned int colour_buffer = 0;
	//unsigned int boneID_buffer = 0;
	//unsigned int boneWT_buffer = 0;

	std::shared_ptr<Material> m_mat;	// a material is stored along side the mesh

public:
	Mesh(std::shared_ptr<Material> material,
		size_t				index_size,
		size_t				vertex_size,
		const unsigned int*	index_data,
		const float*		vertex_data,
		const float*		tCoord_data = nullptr,
		const float*		normal_data = nullptr,
		const float*		colour_data = nullptr
		//std::vector<glm::ivec4>*	boneID_data = nullptr,
		//std::vector<glm::vec4>*	boneWT_data = nullptr
		);

	Mesh(std::shared_ptr<Material> material, 
		std::vector<unsigned int>&	index_data,
		std::vector<float>&			vertex_data,
		std::vector<float>*			tCoord_data = nullptr,
		std::vector<float>*			normal_data = nullptr,
		std::vector<float>*			colour_data = nullptr
	) : Mesh(material, 
		index_data.size() * 4,
		vertex_data.size() * 4, 
		&index_data[0],
		&vertex_data[0], 
		tCoord_data ? &(*tCoord_data)[0] : nullptr, 
		normal_data ? &(*normal_data)[0] : nullptr,
		colour_data ? &(*colour_data)[0] : nullptr) { }
	~Mesh();

	// 
};


