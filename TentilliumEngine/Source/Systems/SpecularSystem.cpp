#include "SpecularSystem.h"
#include "../Components/Transform.h"
#include <glew.h>
#include <glfw3.h>

SpecularSystem::SpecularSystem()
{
	m_program.setUniformBlock("material", 0); // material uniform buffer block set to 0

	on_construct<VBO<Index>>().connect<attachVBO<Index>>();
	on_construct<VBO<Position>>().connect<attachVBO<Position>>();
	on_construct<VBO<TexCoord>>().connect<attachVBO<TexCoord>>();
	on_construct<VBO<Normal>>().connect<attachVBO<Normal>>();
	on_construct<VBO<BoneID>>().connect<attachVBO<BoneID>>();
	on_construct<VBO<BoneWeight>>().connect<attachVBO<BoneWeight>>();

	on_destroy<VBO<Index>>().connect<detachVBO<Index>>();
	on_destroy<VBO<Position>>().connect<detachVBO<Position>>();
	on_destroy<VBO<TexCoord>>().connect<detachVBO<TexCoord>>();
	on_destroy<VBO<Normal>>().connect<detachVBO<Normal>>();
	on_destroy<VBO<BoneID>>().connect<detachVBO<BoneID>>();
	on_destroy<VBO<BoneWeight>>().connect<detachVBO<BoneWeight>>();
}

void SpecularSystem::render()
{
	glm::mat4 view = get_cam_view();
	glm::mat4 proj = get_cam_proj();

	m_program.bind();
	for (auto [entity, material, vao] : specular_view.each())
	{
		if (auto transform = try_get<Transform>(entity))
			m_program.setUniformMat4("MVP", (proj * view) * transform->getWorldMatrix()); 

		material.bind();
		vao.draw();		
	}
}
