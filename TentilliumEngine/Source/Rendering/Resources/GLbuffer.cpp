#include "GLbuffer.h"
#include <glew.h>
#include <glfw3.h>

GLbuffer::GLbuffer(void* data, size_t size, int usage) {
	glCreateBuffers(1, &m_handle);
    glNamedBufferData(m_handle, size, data, usage);
    create(m_handle);
}

GLbuffer::~GLbuffer() {
    if (destroy(m_handle))
        glDeleteBuffers(1, &m_handle);
}

GLbuffer::GLbuffer(const GLbuffer& other) {
    create(other.m_handle);

    if (destroy(m_handle))
        glDeleteBuffers(1, &m_handle);

    m_handle = other.m_handle;
    
}

GLbuffer& GLbuffer::operator=(const GLbuffer& other) {
    if (this == &other)
        return *this; // calls copy constructor

    create(other.m_handle);

    if (destroy(m_handle))
        glDeleteBuffers(1, &m_handle);

    m_handle = other.m_handle;
    return *this;
}

void GLbuffer::set_data(void* data, size_t size, size_t offset) const
{
	glNamedBufferSubData(m_handle, offset, size, data);
}

void GLbuffer::get_data(void* data, size_t size, size_t offset) const
{
	glGetNamedBufferSubData(m_handle, offset, size, data);
}

