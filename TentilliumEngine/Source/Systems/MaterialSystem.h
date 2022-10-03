#pragma once
#include "entt_macros.h"
#include "../Components/Material.h"
#include "../Rendering/Resources/Buffer.h"
#include <set>



class MaterialSystem : virtual protected entt::registry
{
	friend class RenderSystem;
public:
	MaterialSystem();

private:
	struct Members
	{
		std::vector<unsigned int> m_holes;
		unsigned int			  m_size;
	};

	struct MaterialBuffer : Buffer {
		MaterialBuffer() : Buffer(nullptr, sizeof(Material::UniformData) * MAX_MATERIAL_COUNT) { }
	};

	static void createMaterial(entt::registry& reg, entt::entity e);

	static void updateMaterial(entt::registry& reg, entt::entity e);

	static void destroyMaterial(entt::registry& reg, entt::entity e);
};
