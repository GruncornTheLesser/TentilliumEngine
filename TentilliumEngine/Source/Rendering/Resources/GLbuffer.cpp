#include "GLbuffer.h"
#include <glew.h>
#include <glfw3.h>

GLbuffer::GLbuffer(void* data, size_t size) {
	glCreateBuffers(1, &m_handle);
    glNamedBufferData(m_handle, size, data, GL_STATIC_DRAW);
    refCount[m_handle] = 1;
}

GLbuffer::~GLbuffer() {
    if (--refCount[m_handle] == 0) {
        refCount.erase(m_handle);
        glDeleteBuffers(1, &m_handle);
    }
}

GLbuffer::GLbuffer(const GLbuffer& other) {
    m_handle = other.m_handle;
    refCount[m_handle]++;
    
}

GLbuffer& GLbuffer::operator=(const GLbuffer& other) {
    if (this != &other)
    {
        if (--refCount[m_handle] == 0) {
            refCount.erase(m_handle);
            glDeleteBuffers(1, &m_handle);
        }

        m_handle = other.m_handle;
        refCount[m_handle]++;
    }

    return *this;
}

GLbuffer::GLbuffer(const GLbuffer&& other) {
    m_handle = other.m_handle;
    refCount[m_handle]++;
}
GLbuffer& GLbuffer::operator=(const GLbuffer&& other) {
    if (this != &other)
    {
        if (--refCount[m_handle] == 0) {
            refCount.erase(m_handle);
            glDeleteBuffers(1, &m_handle);
        }

        m_handle = other.m_handle;
        refCount[m_handle]++;
    }

    return *this;
}

void GLbuffer::set_data(void* data, size_t size, size_t offset) const
{
	glNamedBufferSubData(m_handle, offset, size, data);
}

void GLbuffer::get_data(size_t offset, void* data, size_t size) const
{
	glGetNamedBufferSubData(m_handle, offset, size, data);
}




void GLbuffer::bind_uniform_slot(unsigned int index)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, m_handle);
}
