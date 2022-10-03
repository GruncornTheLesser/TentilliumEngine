#include "MaterialSystem.h"

MaterialSystem::MaterialSystem()
{
	ctx().emplace<Members>();
	ctx().emplace<ResourceManager<Material::Index, size_t>>();
	ctx().emplace<MaterialBuffer>();
	
	on_construct<Material>().connect<createMaterial>();
	on_destroy<Material>().connect<destroyMaterial>();
}

void MaterialSystem::createMaterial(entt::registry& reg, entt::entity e)
{
	auto& sysMembers = reg.ctx().at<Members>();
	auto& resManager = reg.ctx().at<ResourceManager<Material::Index, size_t>>();
	auto& matBuffer =  reg.ctx().at<MaterialBuffer>();

	Material& material = reg.get<Material>(e);

	for (int i = 0; i < material.m_data.size(); i++) {
		auto fetched = resManager.fetch(std::hash<Material::Data>{}(material.m_data[i]));
		if (fetched.has_value())
			material.m_indices[i] = fetched.value();
		else 
		{
			if (sysMembers.m_holes.size() > 0) 
			{
				material.m_indices[i] = Material::Index(sysMembers.m_holes[sysMembers.m_holes.size() - 1]);
				sysMembers.m_holes.pop_back();
			}
			else 
			{
				material.m_indices[i] = sysMembers.m_size;
				sysMembers.m_size++;
			}

			auto offset = sizeof(Material::UniformData) * material.m_indices[i].value;
			matBuffer.setData(material.m_data[i].toUniformData(), offset);
		}
	}

	std::vector<Material::UniformData> test(sysMembers.m_size);
	matBuffer.getData(test.data(),sizeof(Material::UniformData) * sysMembers.m_size, 0);
}

void MaterialSystem::updateMaterial(entt::registry& reg, entt::entity e)
{
}

void MaterialSystem::destroyMaterial(entt::registry& reg, entt::entity e)
{
	auto& sysMembers = reg.ctx().at<Members>();
	auto& resManager = reg.ctx().at<ResourceManager<Material::Index, size_t>>();

	Material& material = reg.get<Material>(e);

	for (int i = 0; i < material.m_data.size(); i++) {
		Material::Index index = resManager.fetch(std::hash<Material::Data>{}(material.m_data[i])).value();
		if (index.isUnique())
		{
			if (index.value == sysMembers.m_size - 1)
				sysMembers.m_size--;
			else
				sysMembers.m_holes.push_back(index.value);
		}
	}
}
