#include "Material.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

Material::UniformData Material::Data::toUniformData()
{
	UniformData data;

	data.diffuseHasMap = diffuse.index() == 0;
	if (data.diffuseHasMap) data.diffuseMap = std::get<Texture>(diffuse).getBindless();
	else					data.diffuse = std::get<glm::vec3>(diffuse);

	data.opacityHasMap = opacity.index() == 0;
	if (data.opacityHasMap) data.opacityMap = std::get<Texture>(opacity).getBindless();
	else					data.opacity = std::get<float>(opacity);

	data.specularHasMap = specular.index() == 0;
	if (data.specularHasMap) data.specularMap = std::get<Texture>(specular).getBindless();
	else					 data.specular = std::get<glm::vec3>(specular);

	data.shininessHasMap = shininess.index() == 0;
	if (data.shininessHasMap) data.shininessMap = std::get<Texture>(shininess).getBindless();
	else					  data.shininess = std::get<float>(shininess);

	data.emissiveHasMap = emissive.index() == 0;
	if (data.emissiveHasMap) data.emissiveMap = std::get<Texture>(emissive).getBindless();
	else					 data.emissive = std::get<glm::vec3>(emissive);

	data.ambientOcclusionHasMap = ambientOcclusion.has_value();
	if (data.ambientOcclusionHasMap) data.ambientOcclusionMap = ambientOcclusion.value().getBindless();

	data.normalHasMap = normal.has_value();
	if (data.normalHasMap) data.normalMap = normal.value().getBindless();

	data.heightHasMap = height.has_value();
	if (height.has_value()) data.heightMap = height.value().getBindless();

	return data;
}
