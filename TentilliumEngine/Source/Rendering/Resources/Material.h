#pragma once
#include "Resource.h"
#include "Texture.h"
#include <variant>
#include <unordered_map>
#include "GLbuffer.h"
#include "UniformBlock.h"
#include <glm.hpp>

#define MAP(NAME, HASMAP_OFFSET)                                                                            \
public:                                                                                                     \
    __declspec(property(get = get##NAME##Map, put = set##NAME##Map)) std::shared_ptr<Texture> NAME##_map;   \
    void set##NAME##Map(const std::shared_ptr<Texture>& value) {                                            \
        m_uniformBuffer.set((int)(value != nullptr), HASMAP_OFFSET);                                        \
        m_##NAME##_map = value;                                                                             \
    }                                                                                                       \
    std::shared_ptr<Texture> get##NAME##Map() { return m_##NAME##_map; }                                    \
private:                                                                                                    \
    std::shared_ptr<Texture> m_##NAME##_map;

#define COL(NAME, TYPE, VAL_OFFSET, HASMAP_OFFSET)              \
public:                                                         \
    __declspec(property(put = set##NAME##Val)) TYPE NAME##_val; \
    void set##NAME##Val(TYPE value) {                           \
        m_uniformBuffer.set(0, HASMAP_OFFSET);                  \
        m_uniformBuffer.set(value, VAL_OFFSET);                 \
    }






/*
uniformBlock {
type = vec3    name = diff    align = 16   offset = 0
type = float   name = hasD    align = 4    offset = 12
type = float   name = spec    align = 4    offset = 16
type = float   name = hasS    align = 4    offset = 20
type = float   name = glos    align = 4    offset = 24
type = float   name = hasG    align = 4    offset = 28
size = 32
*/

class Material : public Resource<Material>
{
    COL(Diffuse, glm::vec3, 0, 12);
    MAP(Diffuse, 12);

    COL(Specular, float, 16, 20);
    MAP(Specular, 20);

    COL(Gloss, float, 24, 28);
    MAP(Gloss, 28);

public:
    GLbuffer m_uniformBuffer{ nullptr, 32 };
};

#undef MAP
#undef COL