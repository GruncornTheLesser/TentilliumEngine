#pragma once
#include "Resource.h"
#include "GLbuffer.h"
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
	friend class Model;
private:
	GLbuffer m_buffer_index, 
			 m_buffer_vertex, 
			 m_buffer_tCoord, 
			 m_buffer_normal, 
			 m_buffer_colour;
public:
	Mesh(size_t index_size, size_t vertex_size,
		unsigned int* index_data,
		float* vertex_data,
		float* tCoord_data = nullptr,
		float* normal_data = nullptr,
		float* colour_data = nullptr) 
	{
		if (index_data)  m_buffer_index  = GLbuffer(index_data,	index_size);
		if (vertex_data) m_buffer_vertex = GLbuffer(vertex_data, vertex_size * 3);
		if (tCoord_data) m_buffer_tCoord = GLbuffer(tCoord_data,	vertex_size * 2);
		if (normal_data) m_buffer_normal = GLbuffer(normal_data,	vertex_size * 3);
		if (colour_data) m_buffer_colour = GLbuffer(colour_data,	vertex_size * 3);
	}
};



