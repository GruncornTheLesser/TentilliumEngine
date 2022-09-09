#include "Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

void Material::set_diff(Texture texture)
{
	m_textures.insert(std::pair(0, texture));
	set_data(true, offsetof(UniformData, hasD));
}

void Material::set_diff(glm::vec3 value)
{
	m_textures.erase(0);
	set_data(value, offsetof(UniformData, diff));
	set_data(false, offsetof(UniformData, hasD));
}

void Material::set_spec(Texture texture)
{
	m_textures.insert(std::pair(1, texture));
	set_data(false, offsetof(UniformData, hasS));
}

void Material::set_spec(float value)
{
	m_textures.erase(1);
	set_data(value, offsetof(UniformData, spec));
	set_data(false, offsetof(UniformData, hasS));
}

void Material::set_glos(Texture texture)
{
	m_textures.insert(std::pair(2, texture));
	set_data(false, offsetof(UniformData, hasG));
}

void Material::set_glos(float value)
{
	m_textures.erase(2);
	set_data(value, offsetof(UniformData, glos));
	set_data(false, offsetof(UniformData, hasG));
}

void Material::bind()
{
	GLbuffer::bind_uniform_slot(0);
	
	for (auto [slot, texture] : m_textures)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture.m_handle);
	}
}
