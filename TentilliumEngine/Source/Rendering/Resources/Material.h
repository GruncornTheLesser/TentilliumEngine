#pragma once
#include <glm.hpp>
#include "Resource.h"
#include "GLbuffer.h"

struct Material final : GLbuffer, public Resource<Material>
{
    friend class Model;

    Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shine)
        : GLbuffer(GL_UNIFORM_BUFFER, 12)
    { 
        assign(0, sizeof(glm::vec3), (&ambient));
        assign(4, sizeof(glm::vec3), (&diffuse));
        assign(8, sizeof(glm::vec3), (&specular));
        assign(11, sizeof(float), (&shine));
    }

    void setAmbient(glm::vec3 value) { assign(0, sizeof(glm::vec3), (&value)); }

    void setDiffuse(glm::vec3 value) { assign(4, sizeof(glm::vec3), (&value)); }

    void setSpecular(glm::vec3 value) { assign(8, sizeof(glm::vec3), (&value)); }

    void setShine(float value) { assign(11, sizeof(float), (&value)); }

};

Material* Resource<Material>::Load(std::string filepath);