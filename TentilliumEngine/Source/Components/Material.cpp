#include "Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Material::Material(const InitData& data)
	: Material(data.diffuse, data.opacity, data.specular, data.shininess, data.emissive, data.ambientOcclusion, data.normal, data.height)
{ }

Material::Material(
	std::variant<Texture, glm::vec3> diffuse, 
	std::variant<Texture, float> opacity, 
	std::variant<Texture, glm::vec3> specular, 
	std::variant<Texture, float> shininess, 
	std::variant<Texture, glm::vec3> emissive, 
	std::optional<Texture> ambientOcclusion,
	std::optional<Texture> normal, 
	std::optional<Texture> height)
	: m_uniformBuffer(nullptr, sizeof(UniformData))
{
	UniformData data;

	data.diffuseHasMap = diffuse.index() == 0;
	if (data.diffuseHasMap)
	{
		Texture& map = std::get<Texture>(diffuse);
		data.diffuseMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}
	else 
	{
		data.diffuse = std::get<glm::vec3>(diffuse);
	}

	data.opacityHasMap = opacity.index() == 0;
	if (data.opacityHasMap)
	{
		Texture& map = std::get<Texture>(opacity);
		data.opacityMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}
	else 
	{
		data.opacity = std::get<float>(opacity);
	}

	data.specularHasMap = opacity.index() == 0;
	if (data.specularHasMap)
	{
		Texture& map = std::get<Texture>(specular);
		data.specularMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}
	else
	{
		data.specular = std::get<glm::vec3>(specular);
	}

	data.shininessHasMap = shininess.index() == 0;
	if (data.shininessHasMap)
	{
		Texture& map = std::get<Texture>(shininess);
		data.shininessMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}
	else
	{
		data.shininess = std::get<float>(shininess);
	}

	data.emissiveHasMap = emissive.index() == 0;
	if (data.emissiveHasMap)
	{
		Texture& map = std::get<Texture>(emissive);
		data.emissiveMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}
	else
	{
		data.emissive = std::get<glm::vec3>(emissive);
	}

	data.ambientOcclusionHasMap = ambientOcclusion.has_value();
	if (data.ambientOcclusionHasMap)
	{
		Texture& map = ambientOcclusion.value();
		data.ambientOcclusionMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}

	data.normalHasMap = normal.has_value();
	if (data.normalHasMap)
	{
		Texture& map = normal.value();
		data.normalMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}

	data.heightHasMap = height.has_value();
	if (height.has_value())
	{
		Texture& map = height.value();
		data.heightMap = glGetTextureHandleARB(map.handle);
		map.setBindless(true);
		m_maps.push_back(map);
	}

	m_uniformBuffer.setData(data);
}

void Material::bind(int uniformBufferIndex) const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBufferIndex, m_uniformBuffer.handle);
}
