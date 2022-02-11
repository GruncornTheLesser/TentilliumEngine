#include "UniformBuffer.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

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
	// offset data ptr by 16 bytes because c++ uses std140 class layout.
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, size(), (void*)((int)this + 16), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename t>
char* UniformBuffer<t>::getBufferData()
{
	char data[sizeof(t) - 16];
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glGetBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(t) - 16, (void*)(&data));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return data;
}

template<typename t>
void UniformBuffer<t>::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
}

template<typename t>
unsigned int UniformBuffer<t>::size()
{
	return sizeof(t) - 16;
}