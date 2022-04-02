#include "GLbuffer.h"
#include <glew.h>
#include <glfw3.h>

GLbuffer::GLbuffer(void* data, size_t size)
{
	glCreateBuffers(1, &m_bo);
	glNamedBufferData(m_bo, size, data, GL_STATIC_DRAW);
}

GLbuffer::~GLbuffer()
{
	glDeleteBuffers(1, &m_bo);
}

GLbuffer::GLbuffer(GLbuffer&& other)
{
	this->m_bo = other.m_bo;
	other.m_bo = 0;
}

GLbuffer& GLbuffer::operator=(GLbuffer&& other)
{
	glDeleteBuffers(1, &(this->m_bo));

	this->m_bo = other.m_bo;
	other.m_bo = 0;

	return *this;
}

void GLbuffer::set(size_t offset, void* data, size_t size) const
{
	glNamedBufferSubData(m_bo, offset, size, data);
}

void GLbuffer::get(size_t offset, void* data, size_t size) const
{
	glGetNamedBufferSubData(m_bo, offset, size, data);
}
