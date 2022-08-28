#include "RenderSystem.h"
#include "../Components/Mesh.h"

template<VertAttrib attrib>
static void attachVBO(entt::registry& reg, entt::entity e) {
	auto& vao = reg.get_or_emplace<VAO>(e);
	vao.attach(reg.try_get<VBO<attrib>>(e));
}

template<VertAttrib attrib>
static void detachVBO(entt::registry& reg, entt::entity e) {
	auto& vao = reg.get_or_emplace<VAO>(e);
	vao.detach<attrib>();
}

glm::mat4 RenderSystem::get_cam_view()
{
	Transform* proj;
	if (valid(camera) && (proj = try_get<Transform>(camera)))
		return glm::inverse(proj->getWorldMatrix());
	else
		return glm::mat4();
}

glm::mat4 RenderSystem::get_cam_proj()
{
	Projection* view;
	if (valid(camera) && (view = try_get<Projection>(camera)))
		return view->get();
	else
		return glm::mat4();
}

void RenderSystem::resize(int width, int height)
{
	if (width == 0 || height == 0) return;

	if (auto ptr = try_get<Projection>(camera))
	{
		ptr->m_ratio = ((float)width) / height;
		ptr->m_proj = glm::perspective(ptr->m_fovY, ptr->m_ratio, ptr->m_zNear, ptr->m_zFar);
	}
}

RenderSystem::RenderSystem()
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

void RenderSystem::render()
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
