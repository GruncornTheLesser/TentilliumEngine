#include "RenderSystem.h"
#include "../Components/Mesh.h"

#include <glew.h>
#include <glfw3.h>
/*
#define CLUSTER_MAX_LIGHTS 256		// the maximum number of lights per cluster
#define SCENE_MAX_LIGHTS 1048576	// the maximum number of lights in the entire scene
#define OPTIMUM_TILE_COUNT 128		// the target number of clusters per depth
#define MAX_CLUSTER_COUNT 4096		// the maximum number of clusters
*/
#define CLUSTER_MAX_LIGHTS 256		// the maximum number of lights per cluster
#define SCENE_MAX_LIGHTS 65536		// the maximum number of lights in the entire scene
#define OPTIMUM_TILE_COUNT 144		// the target number of clusters per depth
#define MAX_CLUSTER_COUNT 3456		// the maximum number of clusters

#define CALC_CLUSTER_SIZE(size) std::sqrtf(OPTIMUM_TILE_COUNT * size.x / size.y), \
								std::sqrtf(OPTIMUM_TILE_COUNT * size.y / size.x), \
								MAX_CLUSTER_COUNT / OPTIMUM_TILE_COUNT

/* TODO:
* > Light Chunk Management
* 
* > Camera Screen Buffer Update
* 
* > cluster_prepass				*DONE*
*	> .comp script				*DONE*
*	> verify buffers/bugfix		*DONE*
* 
* > depth framebuffer & prepass
*   > depth prepass shader		*DONE*
* 
* > cluster_culling
*	> .comp script
*   > verify buffers/bugfix		
* 
* > cluster_light_culling
*   > .comp script				*DONE*
*	> verify buffers/bugfix 
* 
* > cluster_shading
*	> .vert script				*DONE*
*	> material rendering
*	> .frag script 
*	> verify buffers/bugfix 
*	
*/

struct RenderData {
	glm::mat4 invProj;
	glm::uvec3 clusterSize;
	float bias;
	glm::uvec2 screenSize;
	float scale;
	float zNear;
	float zFar;
};

struct AABB {
	glm::vec4 min;
	glm::vec4 max;
};

struct LightArray {
	unsigned int begin;
	unsigned int end;
};

RenderSystem::RenderSystem() :
	m_renderDataBuffer(nullptr, sizeof(RenderData)),
	m_clusterBuffer(nullptr, sizeof(AABB) * MAX_CLUSTER_COUNT, GL_DYNAMIC_DRAW),
	m_pointLightBuffer(nullptr, sizeof(PointLight) * SCENE_MAX_LIGHTS, GL_DYNAMIC_DRAW),
	m_lightIndiceBuffer(nullptr, sizeof(int) * MAX_CLUSTER_COUNT * CLUSTER_MAX_LIGHTS, GL_DYNAMIC_DRAW),
	m_lightArrayBuffer(nullptr, sizeof(LightArray) * MAX_CLUSTER_COUNT),
	m_visibleCountBuffer(nullptr, sizeof(unsigned int))
{
	on_construct<PointLight>().connect<constructLight>(m_pointLightBuffer);
	on_destroy<PointLight>().connect<destroyLight>(m_pointLightBuffer);
	on_update<PointLight>().connect<updateLight>(m_pointLightBuffer);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_renderDataBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_clusterBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_pointLightBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_lightIndiceBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_lightArrayBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_visibleCountBuffer.handle);

	m_depth_framebuffer.attach(Framebuffer::DEPTH, Texture(nullptr, 1, 1, Texture::Format::DEPTH));

#ifdef CLUSTER_DEBUG
	m_debug_cluster_vao.attach(V_Custom, &m_lightArrayBuffer, 2, GL_UNSIGNED_INT, false, 0);
#endif
}

void RenderSystem::setSize(glm::ivec2 size)
{
	if (size.x == 0 || size.y == 0)  return;

	m_size = (glm::uvec2)size;
	m_clusterSize = glm::uvec3(CALC_CLUSTER_SIZE(size));

	if (!valid(m_camera)) return;
	auto proj = try_get<Projection>(m_camera);
	if (!proj) return;

	proj->m_ratio = ((float)size.x) / size.y;
	proj->m_proj = glm::perspective(proj->m_fovY, proj->m_ratio, proj->m_zNear, proj->m_zFar);

	m_depth_framebuffer.resize(size);

	float scale = m_clusterSize.z / std::log2f(proj->m_zFar / proj->m_zNear);
	float bias = m_clusterSize.z * -std::log2f(proj->m_zNear) / std::log2f(proj->m_zFar / proj->m_zNear);

	m_renderDataBuffer.set_data(RenderData{
		glm::inverse(proj->m_proj),
		m_clusterSize,
		bias,
		m_size,
		scale,
		proj->m_zNear,
		proj->m_zFar,		
		});

	m_prepass.dispatch(m_clusterSize);
}

glm::ivec2 RenderSystem::getSize() 
{ 
	return m_size; 
}

void RenderSystem::setCamera(entt::entity e)
{
	m_camera = e;
	auto [proj, view] = get<Projection, Transform::WorldMatrix>(camera);
	
	float scale = m_clusterSize.z / std::log2f(proj.m_zFar / proj.m_zNear);
	float bias = m_clusterSize.z * -std::log2f(proj.m_zNear) / std::log2f(proj.m_zFar / proj.m_zNear);

	m_renderDataBuffer.set_data(RenderData{
		glm::inverse(proj.m_proj),
		m_clusterSize,
		bias,
		m_size,
		scale,
		proj.m_zNear,
		proj.m_zFar,
		});
	
	m_prepass.dispatch(m_clusterSize);
}

entt::entity RenderSystem::getCamera()
{
	return m_camera;
}

void RenderSystem::render()
{
	// get camera view
	glm::mat4 proj = get<Projection>(camera);
	glm::mat4 view = get<Transform::WorldMatrix>(camera);
	view = glm::inverse(view);

	// depth prepass
	/*
	m_depth_framebuffer.bind();
	m_depth_prepass.bind();
	for (auto [entity, mesh, material, transform] : render_scene_view.each())
		mesh.draw();
	Framebuffer::unbind();
	*/

	// light culling
	m_culling.bind();
	m_culling.setUniformMat4("view", view);
	m_culling.setUniform1i("lightCount", (int)storage<PointLight>().size());
	m_culling.dispatch(glm::uvec3(1, 1, m_clusterSize.z));
	
	m_shading.bind();
	for (auto [entity, mesh, material, model] : render_scene_view.each())
	{
		m_shading.setUniformMat4("model", model);
		m_shading.setUniformMat4("MVP", proj * view * (glm::mat4)model);
		material.bind();
		mesh.draw();
	};

#if defined(CLUSTER_DEBUG)
	glDisable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_CULL_FACE);

	m_debug_cluster_program.bind();
	m_debug_cluster_program.setUniformMat4("VP", proj * view);
	m_debug_cluster_vao.draw(GL_LINES, m_clusterSize.x * m_clusterSize.y * m_clusterSize.z);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	
	
#endif
}

void RenderSystem::constructLight(GLbuffer& m_pointLightBuffer, entt::registry& reg, entt::entity e)
{
	if (reg.storage<PointLight>().size() == SCENE_MAX_LIGHTS)
		throw std::exception();

	// add light to end of buffer array
	auto back = reg.storage<PointLight>().size() - 1;

	m_pointLightBuffer.set_data(reg.get<PointLight>(e), back * sizeof(PointLight));
}

void RenderSystem::destroyLight(GLbuffer& m_pointLightBuffer, entt::registry& reg, entt::entity e)
{
	size_t index = reg.storage<PointLight>().index(e);
	size_t back = reg.storage<PointLight>().size() - 1;
	if (index == back) return; 

	// remove light by swapping with last light
	m_pointLightBuffer.set_data(reg.get<PointLight>(reg.storage<PointLight>().at(back)), index * sizeof(PointLight));
}

void RenderSystem::updateLight(GLbuffer& m_pointLightBuffer, entt::registry& reg, entt::entity e) {
	auto index = reg.storage<PointLight>().index(e);
	m_pointLightBuffer.set_data(reg.get<PointLight>(e).m_position, index * sizeof(PointLight) + offsetof(PointLight, m_position));
}