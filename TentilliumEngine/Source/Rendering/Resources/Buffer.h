#pragma once
#include <unordered_map>
#include "Resource.h"
class Buffer : public GL<Buffer>
{
    friend class GL<Buffer>;
public:
    __declspec(property(get=getHandle)) unsigned int handle;

    Buffer(const void* data, size_t size, int usage = 0x88e4);

    template<typename T>
    Buffer(T data) : Buffer(&data, sizeof(T)) { }

    void setData(const void* data, size_t size, size_t offset) const;

    template<typename t>
    void setData(const t& data, size_t offset = 0) const {
        return setData(&data, sizeof(t), offset);
    }

    void getData(void* data, size_t size, size_t offset) const;

    template<typename t>
    void getData(t* data, size_t offset = 0) const {
        return getData(data, sizeof(t), offset);
    }

    int length() const;

private:
    static void destroy(unsigned int handle);
};

