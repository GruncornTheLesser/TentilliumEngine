#include "SpecularSystem.h"
#include "../Components/Transform.h"
#include <glew.h>
#include <glfw3.h>

void SpecularSystem::initVAO(entt::registry& reg, entt::entity e) {
	reg.emplace_or_replace<VAO>(e, 
		reg.try_get<VBO<VertAttrib::Index>>(e),
		reg.try_get<VBO<VertAttrib::Position>>(e),
		reg.try_get<VBO<VertAttrib::Normal>>(e),
		reg.try_get<VBO<VertAttrib::TexCoord>>(e),
		reg.try_get<VBO<VertAttrib::BoneWeight>>(e),
		reg.try_get<VBO<VertAttrib::BoneID>>(e)		
		);
}

SpecularSystem::SpecularSystem()
{
	m_program.setUniformBlock("material", 0); // material uniform buffer block set to 0

	on_construct<VBO<VertAttrib::Index>>().connect<initVAO>();
	on_destroy<VBO<VertAttrib::Index>>().connect<initVAO>();

	on_construct<VBO<VertAttrib::Position>>().connect<initVAO>();
	on_destroy<VBO<VertAttrib::Position>>().connect<initVAO>();

	on_construct<VBO<VertAttrib::TexCoord>>().connect<initVAO>();
	on_destroy<VBO<VertAttrib::TexCoord>>().connect<initVAO>();

	on_construct<VBO<VertAttrib::Normal>>().connect<initVAO>();
	on_destroy<VBO<VertAttrib::Normal>>().connect<initVAO>();
	
	on_construct<VBO<VertAttrib::BoneID>>().connect<initVAO>();
	on_destroy<VBO<VertAttrib::BoneID>>().connect<initVAO>();

	on_construct<VBO<VertAttrib::BoneWeight>>().connect<initVAO>();
	on_destroy<VBO<VertAttrib::BoneWeight>>().connect<initVAO>();
	
	
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
