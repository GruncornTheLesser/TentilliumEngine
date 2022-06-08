#include "SpecularSystem.h"
#include "../Components/Transform.h"
#include <glew.h>
#include <glfw3.h>

void SpecularSystem::initVAO(entt::registry& reg, entt::entity e) {

	SpecularMaterial* material = reg.try_get<SpecularMaterial>(e);

	if (!material) return;

	VBO<VertAttrib::Index>* vbo_index = reg.try_get<VBO<VertAttrib::Index>>(e);
	VBO<VertAttrib::Position>* vbo_position = reg.try_get<VBO<VertAttrib::Position>>(e);
	VBO<VertAttrib::Normal>* vbo_normal = reg.try_get<VBO<VertAttrib::Normal>>(e);
	VBO<VertAttrib::TexCoord>* vbo_texcoord = reg.try_get<VBO<VertAttrib::TexCoord>>(e);
	VBO<VertAttrib::BoneWeight>* vbo_weight = reg.try_get<VBO<VertAttrib::BoneWeight>>(e);
	VBO<VertAttrib::BoneID>* vbo_boneid = reg.try_get<VBO<VertAttrib::BoneID>>(e);

	if (material->m_vao)
		glDeleteVertexArrays(1, &material->m_vao);

	glGenVertexArrays(1, &material->m_vao);
	glBindVertexArray(material->m_vao);

	if (vbo_index) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index->handle);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &material->m_size);
		material->m_size /= 4;
	}
	else {
		material->m_size = 0;
	}

	if (vbo_position && vbo_normal)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position->handle);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_normal->handle);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		if (vbo_texcoord)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord->handle);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(2);
		}

		if (vbo_weight && vbo_boneid)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo_boneid->handle);
			glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(3);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_weight->handle);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(4);
		}

		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
}

SpecularSystem::SpecularSystem()
{
	m_program.setUniformBlock("material", 0); // material uniform buffer block set to 0

	on_construct<SpecularMaterial>().connect<initVAO>();

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
	for (auto [entity, material] : specular_view.each())
	{
		if (auto transform = try_get<Transform>(entity)) {
			glm::mat4 mvp = transform->getWorldMatrix() * view * proj;
			m_program.setUniformMat4("MVP", mvp);
		}
		
		glBindVertexArray(material.m_vao);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, material.m_uniform_buffer.handle);

		if (material.m_size != 0)
			glDrawElements(GL_TRIANGLES, material.m_size, GL_UNSIGNED_INT, NULL);
		else 
			glDrawArrays(GL_TRIANGLES, 0, GL_UNSIGNED_INT);
	}
}
