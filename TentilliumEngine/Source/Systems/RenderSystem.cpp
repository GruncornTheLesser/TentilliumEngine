#include "RenderSystem.h"
#include "../Components/Mesh.h"

#include <glew.h>
#include <glfw3.h>

#define CLUSTER_MAX_LIGHT 256
#define LIGHTS_CHUNK_SIZE 256
#define CALC_WORKGROUPS_N(size) (int)std::sqrtf(144.0f * size.x / size.y), (int)std::sqrtf(144.0f * size.y / size.x), 24

/* TODO:
* > Light Chunk Management
* > Camera Screen Buffer Update
* > cluster_prepass				*DONE*
*	> .comp script				*DONE*
*	> verify buffers/bugfix		*DONE*
* 
* > cluster_culling
*   > depth prepass shader		*DONE*
*   > depth framebuffer
*   > depth prepass
*	> .comp script
* 
* > cluster_light_culling
*   > .comp script				*DONE*
*	> verify buffers/bugfix 
* > cluster_shading
*	> .vert script				*DONE*
*	> .frag script 
*	> verify buffers/bugfix 
*	
*/

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

struct AABB {
	glm::vec4 min;
	glm::vec4 max;
};

struct ScreenData {
	glm::mat4 inverseProjection;
	float zNear;
	float zFar;
	glm::uvec2 screenSize;
};

RenderSystem::RenderSystem(glm::ivec2 size) :
	m_workGroups(CALC_WORKGROUPS_N(size)),
	m_ScreenBuffer(ScreenData{ glm::mat4(), 0, 0, (glm::uvec2)size }),
	m_clusterBuffer(nullptr, sizeof(AABB) * m_workGroups.x * m_workGroups.y * m_workGroups.z),
	m_ptLightBuffer(nullptr, sizeof(PointLight) * LIGHTS_CHUNK_SIZE, GL_DYNAMIC_DRAW),
	m_visibleBuffer(nullptr, sizeof(int) * CLUSTER_MAX_LIGHT * m_workGroups.x * m_workGroups.y * m_workGroups.z, GL_DYNAMIC_DRAW)
{
	//m_program.setUniformBlock("material", 0);

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

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ScreenBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_clusterBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ptLightBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_visibleBuffer.handle);

	// testing
	glBindVertexArray(m_line_vao.handle);
	glBindBuffer(GL_ARRAY_BUFFER, m_clusterBuffer.handle);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}



void RenderSystem::resize(glm::ivec2 size)
{
	if (size.x == 0 || size.y == 0)  return;

	auto proj = try_get<Projection>(m_camera);
	if (!proj) throw std::exception();

	proj->m_ratio = ((float)size.x) / size.y;
	proj->m_proj = glm::perspective(proj->m_fovY, proj->m_ratio, proj->m_zNear, proj->m_zFar);
	
	m_ScreenBuffer.set_data(ScreenData{
		glm::inverse(proj->m_proj),
		proj->m_zNear,
		proj->m_zFar,
		(glm::uvec2)size,
	});

	m_workGroups = glm::uvec3(CALC_WORKGROUPS_N(size));
	m_prepass.bind();
	glDispatchCompute(m_workGroups.x, m_workGroups.y, m_workGroups.z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void RenderSystem::setCamera(entt::entity e)
{
	if (!valid(m_camera)) throw std::exception();
	
	auto [proj, view] = try_get<Projection, Transform>(e);
	if (!proj || !view) throw std::exception();
	
	m_camera = e;

	ScreenData data = ScreenData{ glm::inverse(proj->m_proj), proj->m_zNear, proj->m_zFar };
	m_ScreenBuffer.set_data(&data, sizeof(ScreenData) - sizeof(glm::uvec2), 0);

	m_prepass.bind();
	m_prepass.dispatch(m_workGroups);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

entt::entity RenderSystem::getCamera()
{
	return m_camera;
}

void RenderSystem::render()
{
	// get camera
	if (!valid(m_camera)) return;
	// get camera projection
	auto proj_comp = try_get<Projection>(m_camera);
	if (!proj_comp) throw std::exception();
	glm::mat4 proj = proj_comp->get();
	// get camera view
	auto view_comp = try_get<Transform>(m_camera);
	if (!view_comp) throw std::exception();
	glm::mat4 view = glm::inverse(view_comp->getWorldMatrix());
	
	m_program.bind();
	for (auto [entity, material, vao] : render_view.each())
	{
		if (auto transform = try_get<Transform>(entity))
			m_program.setUniformMat4("MVP", proj * view * transform->getWorldMatrix());

		int x = Resource<Texture>::count(2);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, material.handle);
		for (auto& [slot, texture] : material.m_textures) 
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, texture.handle);
		}
		vao.draw();
	}
	
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(10);

	glDisable(GL_DEPTH_TEST);

	m_line_program.bind();
	m_line_program.setUniformMat4("VP", proj * view);
	glBindVertexArray(m_line_vao.handle);
	glDrawArrays(GL_POINTS, 0, 2 * m_workGroups.x * m_workGroups.y * m_workGroups.z);

	glEnable(GL_DEPTH_TEST);
}