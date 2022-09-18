#include "Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

void Material::set_diffuse(Texture texture)
{
	m_textures.insert(std::pair(0, texture));
	set_data(true, offsetof(UniformData, hasDiffuseMap));
}

void Material::set_diffuse(glm::vec4 value)
{
	m_textures.erase(0);
	set_data(value, offsetof(UniformData, diffuse));
	set_data(false, offsetof(UniformData, hasDiffuseMap));
}

void Material::set_specular(Texture texture)
{
	m_textures.insert(std::pair(1, texture));
	set_data(false, offsetof(UniformData, hasSpecularMap));
}

void Material::set_specular(glm::vec4 value)
{
	m_textures.erase(1);
	set_data(value, offsetof(UniformData, specular));
	set_data(false, offsetof(UniformData, hasSpecularMap));
}

void Material::set_normal(Texture texture)
{
	m_textures.insert(std::pair(2, texture));
	set_data(false, offsetof(UniformData, hasNormalMap));
}

void Material::bind(int index, int texture_slot_offset) const {
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, GLbuffer::get_handle());
	for (auto& [slot, texture] : m_textures)
	{
		glActiveTexture(GL_TEXTURE0+ texture_slot_offset + slot);
		glBindTexture(GL_TEXTURE_2D, texture.handle);
	}
}