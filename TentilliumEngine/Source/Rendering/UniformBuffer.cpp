#include "UniformBuffer.h"
#include "Resources/Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
//#include <iostream>

#ifdef WIN32
constexpr unsigned int buffer_offset = 16;
#else
constexpr unsigned int buffer_offset = 4;
#endif

template<typename t>
UniformBuffer<t>::UniformBuffer()
{
	glGenBuffers(1, &UBO);
}

template<typename t>
UniformBuffer<t>::~UniformBuffer()
{
	glDeleteBuffers(1, &UBO);
}

template<typename t>
void UniformBuffer<t>::setBufferData()
{
	/*
	std::cout << "sizeof(Material) = " << sizeof(t) << std::endl
		<< "sizeof(UniformBuffer<void>) = " << sizeof(UniformBuffer<void>) << std::endl;
	int i;
	for (i = 0; i < (sizeof(t) - buffer_offset) / sizeof(float) - 1; i++)
		std::cout << ((float*)((size_t)this + buffer_offset))[i] << ", ";
	std::cout << ((float*)((size_t)this + buffer_offset))[i] << std::endl;
	*/
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(t) - buffer_offset, (void*)((size_t)this + buffer_offset), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename t>
char* UniformBuffer<t>::getBufferData()
{
	char* out_data = new char[sizeof(t) - buffer_offset];
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glGetBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(t) - buffer_offset, out_data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return out_data;
}

template<typename t>
void UniformBuffer<t>::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
}


template struct UniformBuffer<Material>;