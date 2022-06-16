#include "GLbuffer.h"
#include <glew.h>
#include <glfw3.h>

GLbuffer::GLbuffer(void* data, size_t size) : m_refData(new RefCounter{ 1 }) { 
	glCreateBuffers(1, &m_handle);
	glNamedBufferData(m_handle, size, data, GL_STATIC_DRAW);
}

GLbuffer::~GLbuffer() {
    if (m_refData->m_refCount-- == 0) {
        delete m_refData;
        glDeleteBuffers(1, &m_handle);
    }
}

GLbuffer::GLbuffer(const GLbuffer& other) {
    m_handle = other.m_handle;
    m_refData = other.m_refData;
    m_refData->m_refCount++;
    
}

GLbuffer& GLbuffer::operator=(const GLbuffer& other) {
    if (this != &other)
    {
        if (m_refData->m_refCount-- == 0) {
            delete m_refData;
            glDeleteBuffers(1, &m_handle);
        }

        m_handle = other.m_handle;
        m_refData = other.m_refData;
        m_refData->m_refCount++;
    }

    return *this;
}

GLbuffer::GLbuffer(const GLbuffer&& other) {
    m_handle = other.m_handle;
    m_refData = other.m_refData;
    m_refData->m_refCount++;
}
GLbuffer& GLbuffer::operator=(const GLbuffer&& other) {
    if (this != &other)
    {
        if (m_refData->m_refCount-- == 0) {
            delete m_refData;
            glDeleteBuffers(1, &m_handle);
        }

        m_handle = other.m_handle;
        m_refData = other.m_refData;
        m_refData->m_refCount++;
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
