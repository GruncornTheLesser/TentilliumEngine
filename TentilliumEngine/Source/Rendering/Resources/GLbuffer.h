#pragma once
#include <unordered_map>
#include "Resource.h"
class GLbuffer : Resource<GLbuffer>
{
public:
    __declspec(property(get = get_handle)) unsigned int handle;

private:
    unsigned int m_handle;

private:
    GLbuffer(unsigned int handle) : m_handle(handle) { }
public:
    GLbuffer() : m_handle(0) { }

    GLbuffer(void* data, size_t size, int usage = 0x88e4);

    template<typename T>
    GLbuffer(T data) : GLbuffer(&data, sizeof(T)) { }
    
    ~GLbuffer();

    GLbuffer(const GLbuffer& other);
    GLbuffer& operator=(const GLbuffer& other);

public:
    void set_data(void* data, size_t size, size_t offset) const;

    template<typename t>
    void set_data(t* data, size_t offset = 0) const {
        return set_data(data, sizeof(t), offset);
    }

    template<typename t>
    void set_data(t data, size_t offset = 0) const {
        return set_data(&data, sizeof(t), offset);
    }

    void get_data(void* data, size_t size, size_t offset) const;

    template<typename t>
    void get_data(t* data, size_t offset = 0) const {
        return get_data(data, sizeof(t), offset);
    }

    unsigned int get_handle() const {
        return m_handle;
    }
};

