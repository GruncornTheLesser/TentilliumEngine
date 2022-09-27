#include "Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

void Material::bind(int uniformBufferIndex) const {
	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBufferIndex, m_uniformBuffer.handle);
	
	if (m_DiffuseMap.handle != 0) 
		m_DiffuseMap.bindSlot(0);

	if (m_specularMap.handle != 0) 
		m_specularMap.bindSlot(1);

	if (m_glossMap.handle != 0) 
		m_glossMap.bindSlot(2);

	if (m_normalMap.handle != 0) 
		m_normalMap.bindSlot(3);
}

void Material::setDiffuse(const glm::vec4& value)
{
	m_uniformBuffer.setData(value, offsetof(UniformData, diffuse));
	m_uniformBuffer.setData(false, offsetof(UniformData, hasDiffuseMap));
	m_DiffuseMap = Texture(0); // clear Texture value
}

void Material::setDiffuse(const glm::vec3& value)
{
	m_uniformBuffer.setData(glm::vec4(value.x, value.y, value.z, 1), offsetof(UniformData, diffuse));
	m_uniformBuffer.setData(false, offsetof(UniformData, hasDiffuseMap));
	m_DiffuseMap = Texture(0); // clear Texture value
}

void Material::setDiffuse(const Texture& value)
{
	m_uniformBuffer.setData(true, offsetof(UniformData, hasDiffuseMap));
	m_DiffuseMap = value; // set Texture value
}

void Material::setSpecular(float value)
{
	m_uniformBuffer.setData(value, offsetof(UniformData, specular));
	m_uniformBuffer.setData(false, offsetof(UniformData, hasSpecularMap));
	m_specularMap = Texture(0); // clear Texture value
}

void Material::setSpecular(const Texture& value)
{
	m_uniformBuffer.setData(true, offsetof(UniformData, hasSpecularMap));
	m_specularMap = value; // set Texture value
}

void Material::setGloss(float value)
{
	m_uniformBuffer.setData(value, offsetof(UniformData, gloss));
	m_uniformBuffer.setData(false, offsetof(UniformData, hasGlossMap));
	m_glossMap = Texture(0); // clear Texture value
}

void Material::setGloss(const Texture& value)
{
	m_uniformBuffer.setData(true, offsetof(UniformData, hasGlossMap));
	m_glossMap = value; // set Texture value
}

void Material::setNormal(const Texture& value)
{
	m_uniformBuffer.setData(true, offsetof(UniformData, hasNormalMap));
	m_normalMap = value; // clear Texture value
}