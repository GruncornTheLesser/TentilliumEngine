#include "GLbuffer.h"
#include <glew.h>
#include <glfw3.h>

GLbuffer::GLbuffer(void* data, size_t size)
{
	glCreateBuffers(1, &m_handle);
	glNamedBufferData(m_handle, size, data, GL_STATIC_DRAW);
}

GLbuffer::~GLbuffer()
{
	glDeleteBuffers(1, &m_handle);
}

GLbuffer::GLbuffer(GLbuffer&& other)
{
	this->m_handle = other.m_handle;
	other.m_handle = 0;
}

GLbuffer& GLbuffer::operator=(GLbuffer&& other)
{
	glDeleteBuffers(1, &(this->m_handle));

	this->m_handle = other.m_handle;
	other.m_handle = 0;

	return *this;
}

void GLbuffer::setData(void* data, size_t size, size_t offset) const
{
	glNamedBufferSubData(m_handle, offset, size, data);
}

void GLbuffer::getData(size_t offset, void* data, size_t size) const
{
	glGetNamedBufferSubData(m_handle, offset, size, data);
}
