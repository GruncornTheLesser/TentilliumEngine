#pragma once
#include "Resource.h"
#include "Material.h"
#include <vector>

typedef void (*callback_setupVertex)();

template<typename ... ts>
void setupVertex()
{
	void setupAttrib(int size, unsigned int stride, unsigned int& no, unsigned int& offset);

	unsigned int stride = (sizeof(ts) + ...);	// sum all sizes of ts to get the total length
	unsigned int atrribno = 0;					// index of each type
	unsigned int offset = 0;					// pointer to where type starts

	(setupAttrib(sizeof(ts), stride, atrribno, offset), ...);
}

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
	struct VBO
	{
		unsigned int vertex_buffer = 0;
		unsigned int tCoord_buffer = 0;
		unsigned int normal_buffer = 0;
		unsigned int colour_buffer = 0;
		
		//unsigned int boneID_buffer = 0;
		//unsigned int boneWT_buffer = 0;
		
		unsigned int index_buffer = 0;
	};

	std::shared_ptr<Material> m_mat;
	VBO m_vbo; // vertex buffer data

	// construct from assimp in Scene::Load
	Mesh(void* aiMesh, std::shared_ptr<Material> mat);
public:
	Mesh(std::shared_ptr<Material> material,
		std::vector<unsigned int>&	index_data,
		std::vector<float>&			vertex_data,
		std::vector<float>*			tCoord_data = nullptr,
		std::vector<float>*			normal_data = nullptr,
		std::vector<float>*			colour_data = nullptr
		//std::vector<glm::ivec4>*	boneID_data = nullptr,
		//std::vector<glm::vec4>*	boneWT_data = nullptr
		);
};


