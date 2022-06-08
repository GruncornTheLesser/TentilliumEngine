#pragma once

class GLbuffer
{
public:
    __declspec(property(get = get_handle)) unsigned int handle;

protected:
    unsigned int m_handle;

public:
    GLbuffer(void* data, size_t size);
    
    ~GLbuffer();

    GLbuffer(const GLbuffer&) = delete;
    GLbuffer& operator=(const GLbuffer&) = delete;

    GLbuffer(GLbuffer&&);
    GLbuffer& operator=(GLbuffer&&);

    void setData(void* data, size_t size, size_t offset) const;

    template<typename t>
    void setData(t* data, size_t offset = 0) const {
        return setData(data, sizeof(t), offset);
    }

    template<typename t>
    void setData(t data, size_t offset = 0) const {
        return setData(&data, sizeof(t), offset);
    }

    void getData(size_t offset, void* data, size_t size) const;

    template<typename t>
    void getData(t* data, size_t offset = 0) const {
        return getData(offset, data, sizeof(t));
    }

    template<typename t>
    void getData(t data, size_t offset = 0) const {
        return getData(offset, &data, sizeof(t));
    }
    
    unsigned int get_handle() const { 
        return m_handle;
    }
};

