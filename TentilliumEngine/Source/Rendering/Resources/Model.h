#pragma once
#include <vector>
#include "Mesh.h"
#include "Material.h"


struct Model
{
private:
	struct RenderInfo
	{
		unsigned int VAO;
		unsigned int Mat;
		int Size;
	};
	
	std::vector<RenderInfo> meshes;
	

public:
	Model() {}
	Model(std::vector<std::shared_ptr<Mesh>> p_meshes, std::vector<std::shared_ptr<Material>> p_materials);

	void draw();

};
