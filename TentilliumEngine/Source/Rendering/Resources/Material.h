#pragma once
#include <glm.hpp>
#include "Resource.h"
#include "GLbuffer.h"

//TODO: FINISH MATERIAL

struct Material : GLbuffer, public Resource<Material>
{
    friend class Model;

public:
    Material(void* aiPtr) : GLbuffer(GL_UNIFORM_BUFFER, 0) 
    { 
       /*

        auto aiMat = static_cast<aiMaterial*>(aiPtr);
        for (int i = 0; i < aiMat->GetTextureCount(aiTextureType_DIFFUSE); i++)
        {

        }

        auto aiMat = static_cast<aiMaterial*>(aiPtr);
        for (int i = 0; i < aiMat->GetTextureCount(aiTextureType_NORMALS); i++)
        {

        }

        auto aiMat = static_cast<aiMaterial*>(aiPtr);
        for (int i = 0; i < aiMat->GetTextureCount(aiTextureType_AMBIENT); i++)
        {

        }
        */
    }
    Material()
        : GLbuffer(GL_UNIFORM_BUFFER, 0)
    { 

    }

    void setAmbient(glm::vec3 value) { }

    void setDiffuse(glm::vec3 value) { }

    void setSpecular(glm::vec3 value) { }

    void setShine(float value) { }

};