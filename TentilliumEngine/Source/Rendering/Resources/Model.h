#pragma once
#include "Mesh.h"
#include "Material.h"
#include "ShaderProgram.h"
#include <vector>

class Model
{
private:
	class MeshInstance
	{
	public:
		int m_size;
		unsigned int m_VAO;
		
		std::shared_ptr<Mesh> m_mesh;
		std::shared_ptr<Material> m_material;

		MeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
		~MeshInstance();

		MeshInstance(const MeshInstance&) = delete;
		MeshInstance& operator=(const MeshInstance&) = delete;

		MeshInstance(MeshInstance&&);
		MeshInstance& operator=(MeshInstance&&);

		void draw() const;
	};

public:
	std::vector<MeshInstance> m_meshes;

public:
	Model();
	Model(std::vector<std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>>> meshes);

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	Model(Model&&) = default;
	Model& operator=(Model&&) = default;
	
	void add(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	void draw(const ShaderProgram& shaderProgram) const;
};