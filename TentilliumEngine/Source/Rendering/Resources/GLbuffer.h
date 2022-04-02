#pragma once

class GLbuffer
{
public:
    __declspec(property(get = get_handle)) unsigned int handle;

private:
    unsigned int m_bo = 0;

public:
    GLbuffer() { }
    GLbuffer(void* data, size_t size);

    ~GLbuffer();

    GLbuffer(const GLbuffer&) = delete;
    GLbuffer& operator=(const GLbuffer&) = delete;

    GLbuffer(GLbuffer&&);
    GLbuffer& operator=(GLbuffer&&);

    void set(size_t offset, void* data, size_t size) const;

    void get(size_t offset, void* data, size_t size) const;

    template<typename t>
    void set(t* data, size_t offset = 0) const {
        return set(offset, data, sizeof(t)); 
    }

    template<typename t>
    void set(t data, size_t offset = 0) const {
        return set(offset, &data, sizeof(t));
    }

    unsigned int get_handle() const { 
        return m_bo; 
    }
};

