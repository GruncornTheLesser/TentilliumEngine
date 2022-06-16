#include "SpecularSystem.h"
#include "../Components/Transform.h"
#include <glew.h>
#include <glfw3.h>

SpecularSystem::SpecularSystem()
{
	m_program.setUniformBlock("material", 0); // material uniform buffer block set to 0

	on_construct<VBO<VertAttrib::Index>>().connect<attachVBO<VertAttrib::Index>>();
	on_construct<VBO<VertAttrib::Position>>().connect<attachVBO<VertAttrib::Position>>();
	on_construct<VBO<VertAttrib::TexCoord>>().connect<attachVBO<VertAttrib::TexCoord>>();
	on_construct<VBO<VertAttrib::Normal>>().connect<attachVBO<VertAttrib::Normal>>();
	on_construct<VBO<VertAttrib::BoneID>>().connect<attachVBO<VertAttrib::BoneID>>();
	on_construct<VBO<VertAttrib::BoneWeight>>().connect<attachVBO<VertAttrib::BoneWeight>>();

	on_destroy<VBO<VertAttrib::Index>>().connect<detachVBO<VertAttrib::Index>>();
	on_destroy<VBO<VertAttrib::Position>>().connect<detachVBO<VertAttrib::Position>>();
	on_destroy<VBO<VertAttrib::TexCoord>>().connect<detachVBO<VertAttrib::TexCoord>>();
	on_destroy<VBO<VertAttrib::Normal>>().connect<detachVBO<VertAttrib::Normal>>();
	on_destroy<VBO<VertAttrib::BoneID>>().connect<detachVBO<VertAttrib::BoneID>>();
	on_destroy<VBO<VertAttrib::BoneWeight>>().connect<detachVBO<VertAttrib::BoneWeight>>();
	
	
}

void SpecularSystem::render()
{
	glm::mat4 proj = get_cam_proj();
	glm::mat4 view = get_cam_view();

	m_program.bind();
	for (auto [entity, material, vao] : specular_view.each())
	{
		if (auto transform = try_get<Transform>(entity))
			m_program.setUniformMat4("MVP", (view * proj) * transform->getWorldMatrix()); 

		material.bind();
		vao.draw();		
	}
}
