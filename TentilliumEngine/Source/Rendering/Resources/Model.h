#pragma once
#include <vector>
#include "Mesh.h"
#include "Material.h"


class Model
{
private:
	struct RenderInfo
	{
		int size;
		unsigned int VAO;
		~RenderInfo();
	};

	std::shared_ptr<std::vector<RenderInfo>> m_renderInfo;
	std::vector<std::shared_ptr<Mesh>> m_meshes;


public:
	Model(std::vector<std::shared_ptr<Mesh>> p_meshes);
	void draw();
};
