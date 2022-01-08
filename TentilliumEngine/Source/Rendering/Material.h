#pragma once
#include <glm.hpp>	// GL maths
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include "ResourceManager.h"

struct Material
{
private:
    GLuint UBO; // uniform buffer object
    struct UniformBuffer 
    {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec3 specular;
        float specular_power;
    };
public:
    ~Material()
    {
        glDeleteBuffers(1, &UBO);
    }
    void Init(glm::vec3 a, glm::vec3 d, glm::vec3 s, float p)
    {
        if (!UBO)
            glGenBuffers(1, &UBO);
        auto data = UniformBuffer{ glm::vec4(a, 0), glm::vec4(d, 0), s, p };
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBuffer), &data, GL_STATIC_DRAW);
    }
};

const Material* ResourceManager<Material>::Load(const char* filepath) {
    // parse data from file to resource manager
}