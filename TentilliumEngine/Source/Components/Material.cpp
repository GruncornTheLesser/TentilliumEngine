#include "Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Material::UniformData Material::Data::toUniformData()
{
	UniformData data;

	data.diffuseHasMap = diffuse.index() == 0;
	if (data.diffuseHasMap)
	{
		Texture& texture = std::get<Texture>(diffuse);
		data.diffuseMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}
	else
	{
		data.diffuse = std::get<glm::vec3>(diffuse);
	}

	data.opacityHasMap = opacity.index() == 0;
	if (data.opacityHasMap)
	{
		Texture& texture = std::get<Texture>(opacity);
		data.opacityMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}
	else
	{
		data.opacity = std::get<float>(opacity);
	}

	data.specularHasMap = specular.index() == 0;
	if (data.specularHasMap)
	{
		Texture& texture = std::get<Texture>(specular);
		data.specularMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}
	else
	{
		data.specular = std::get<glm::vec3>(specular);
	}

	data.shininessHasMap = shininess.index() == 0;
	if (data.shininessHasMap)
	{
		Texture& texture = std::get<Texture>(shininess);
		data.shininessMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}
	else
	{
		data.shininess = std::get<float>(shininess);
	}

	data.emissiveHasMap = emissive.index() == 0;
	if (data.emissiveHasMap)
	{
		Texture& texture = std::get<Texture>(emissive);
		data.emissiveMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}
	else
	{
		data.emissive = std::get<glm::vec3>(emissive);
	}

	data.ambientOcclusionHasMap = ambientOcclusion.has_value();
	if (data.ambientOcclusionHasMap)
	{
		Texture& texture = ambientOcclusion.value();
		data.ambientOcclusionMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}

	data.normalHasMap = normal.has_value();
	if (data.normalHasMap)
	{
		Texture& texture = normal.value();
		data.normalMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}

	data.heightHasMap = height.has_value();
	if (height.has_value())
	{
		Texture& texture = height.value();
		data.heightMap = glGetTextureHandleARB(texture.handle);
		texture.setBindless(true);
	}

	return data;
}
