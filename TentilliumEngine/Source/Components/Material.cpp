#include "Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Material::Material(
	std::variant<Texture, glm::vec4> diffuse, 
	std::variant<Texture, float> specular, 
	std::variant<Texture, float> gloss, 
	std::optional<Texture> normal)
	: m_uniformBuffer(nullptr, sizeof(UniformData))
{
	UniformData data{ glm::vec4(1, 1, 1, 1), 1, 1, false, false, false, false };
	
	data.hasDiffuseMap = diffuse.index() == 0;
	if (data.hasDiffuseMap) 
		m_diffuseMap = std::get<Texture>(diffuse);
	else					
		data.diffuse = std::get<glm::vec4>(diffuse);
	
	data.hasSpecularMap = specular.index() == 0;
	if (data.hasSpecularMap) 
		m_specularMap = std::get<Texture>(specular);
	else					 
		data.specular = std::get<float>(specular);
	
	data.hasGlossMap = gloss.index() == 0;
	if (data.hasGlossMap)
		m_glossMap = std::get<Texture>(gloss);
	else				 
		data.gloss = std::get<float>(gloss);
	
	data.hasNormalMap = normal.has_value();
	if (data.hasNormalMap) 
		m_normalMap = normal.value();
	
	m_uniformBuffer.setData(data);
} 
void Material::bind(int uniformBufferIndex) const {

	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBufferIndex, m_uniformBuffer.handle);

	if (m_diffuseMap.has_value()) 
		m_diffuseMap.value().bindSlot(0);

	if (m_specularMap.has_value()) 
		m_specularMap.value().bindSlot(1);

	if (m_glossMap.has_value()) 
		m_glossMap.value().bindSlot(2);

	if (m_normalMap.has_value()) 
		m_normalMap.value().bindSlot(3);
}