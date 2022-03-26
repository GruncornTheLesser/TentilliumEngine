#pragma once
#include "Mesh.h"
#include "Material.h"
#include <vector>

class Model
{
private:
	struct MeshInstance
	{
		int size;
		unsigned int VAO;
		std::shared_ptr<Mesh> m_mesh;
		Material m_material;
		// TODO: VAO destruction -> Handle class with crtp so the MeshInstance can be moved but not copied
		MeshInstance(std::shared_ptr<Mesh> mesh, Material material);
	};

	std::vector<MeshInstance> m_meshes;

public:
	Model(std::vector<std::shared_ptr<Mesh>> p_meshes, std::vector<Material> p_materials);

	void draw() const;
};