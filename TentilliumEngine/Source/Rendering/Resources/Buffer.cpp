#include "Buffer.h"
#include <glew.h>
#include <glfw3.h>

Buffer::Buffer(const void* data, size_t size, int usage) {
    glCreateBuffers(1, &m_handle);
    glNamedBufferData(m_handle, size, data, usage);
}

void Buffer::setData(const void* data, size_t size, size_t offset) const
{
    glNamedBufferSubData(m_handle, offset, size, data);
}

void Buffer::getData(void* data, size_t size, size_t offset) const
{
    glGetNamedBufferSubData(m_handle, offset, size, data);
}

int Buffer::length() const
{
    int size;
    glGetNamedBufferParameteriv(m_handle, GL_BUFFER_SIZE, &size);
    return size;
}

void Buffer::destroy(unsigned int handle)
{
    glDeleteBuffers(1, &handle);
}
