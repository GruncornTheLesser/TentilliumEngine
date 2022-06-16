#pragma once

class GLbuffer 
{
private:
    struct RefCounter { unsigned int m_refCount; };
    RefCounter* m_refData;

public:
    __declspec(property(get = get_handle)) unsigned int handle;

protected:
    unsigned int m_handle;

public:
    template<typename T>
    GLbuffer(T data) : GLbuffer(&data, sizeof(T)) { }
    GLbuffer(void* data, size_t size);
    ~GLbuffer();

    GLbuffer(const GLbuffer& other);
    GLbuffer& operator=(const GLbuffer& other);

    GLbuffer(const GLbuffer&& other);
    GLbuffer& operator=(const GLbuffer&& other);

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



    void get_data(size_t offset, void* data, size_t size) const;

    template<typename t>
    void get_data(t* data, size_t offset = 0) const {
        return get_data(offset, data, sizeof(t));
    }

    template<typename t>
    void get_data(t data, size_t offset = 0) const {
        return get_data(offset, &data, sizeof(t));
    }

    void bind_uniform_slot(unsigned int index);

    unsigned int get_handle() const {
        return m_handle;
    }
};

