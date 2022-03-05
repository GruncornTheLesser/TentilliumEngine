#pragma once
#include <vector>
#include "Mesh.h"
#include "Material.h"


struct Model
{
private:
	struct RenderInfo 
	{	
		int size;
		unsigned int VAO;						// non transportable between windows
		std::shared_ptr<Mesh> mesh;				
		std::shared_ptr<Material> material;
		//entt::entity entityMap;

		RenderInfo(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
		~RenderInfo();
	};

	std::shared_ptr<std::vector<RenderInfo>> m_meshes;

public:
	Model(std::vector<std::shared_ptr<Mesh>> p_meshes, std::vector<std::shared_ptr<Material>> p_materials);

	void draw();
};
