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
#define CLUSTER_MAX_LIGHTS 4		// the maximum number of lights per cluster
#define SCENE_MAX_LIGHTS 16			// the maximum number of lights in the entire scene
#define OPTIMUM_TILE_COUNT 16		// the target number of clusters per depth
#define MAX_CLUSTER_COUNT 64		// the maximum number of clusters

#define CALC_CLUSTER_SIZE(size) (int)std::sqrtf(OPTIMUM_TILE_COUNT * (size.x) / size.y), \
								(int)std::sqrtf(OPTIMUM_TILE_COUNT * (size.y) / size.x), \
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
	float zNear;
	float zFar;
	glm::uvec2 screenSize;
	glm::ivec3 clusterSize;
};

struct AABB {
	glm::vec4 min;
	glm::vec4 max;
};

struct LightArray {
	unsigned int index;
	unsigned int count;
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
	/*
	glm::ivec3 max_count, max_size; int max_invocations, max_shared_memory;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_count[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_count[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_count[2]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &max_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &max_size[2]);
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_invocations);
	glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &max_shared_memory);

	std::cout << "max work group count: " << max_count.x << ", " << max_count.y << ", " << max_count.z << std::endl;
	std::cout << "max work group size: " << max_size.x << ", " << max_size.y << ", " << max_size.z << std::endl;
	std::cout << "max work group invocations: " << max_invocations << std::endl;
	std::cout << "max shared memory: " << max_shared_memory << " bytes" << std::endl;
	*/
	m_debug_cluster_vao.attach(V_Custom, &m_lightArrayBuffer, 2, GL_UNSIGNED_INT, false, 0);
#endif
}


void RenderSystem::setSize(glm::ivec2 size)
{
	if (size.x == 0 || size.y == 0)  return;

	m_size = size;
	m_clusterSize = glm::uvec3(CALC_CLUSTER_SIZE(size));

	if (!valid(m_camera)) return;
	auto proj = try_get<Projection>(m_camera);
	if (!proj) return;

	proj->m_ratio = ((float)size.x) / size.y;
	proj->m_proj = glm::perspective(proj->m_fovY, proj->m_ratio, proj->m_zNear, proj->m_zFar);

	m_depth_framebuffer.resize(size);

	m_renderDataBuffer.set_data(RenderData{
		glm::inverse(proj->m_proj),
		proj->m_zNear,
		proj->m_zFar,
		m_size,
		m_clusterSize
		});

	m_prepass.dispatch(m_clusterSize);
}

glm::ivec2 RenderSystem::getSize() 
{ 
	return m_size; 
}

void RenderSystem::setCamera(entt::entity e)
{
	if (!valid(m_camera)) throw std::exception();
	
	auto [proj, view] = try_get<Projection, Transform>(e);
	if (!proj || !view) throw std::exception();
	
	m_camera = e;

	m_renderDataBuffer.set_data(RenderData{ 
		glm::inverse(proj->m_proj), 
		proj->m_zNear, 
		proj->m_zFar, 
		m_size,
		m_clusterSize
		});
	
	m_prepass.dispatch(m_clusterSize);
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
	glm::mat4 proj = *proj_comp;

	// get camera view
	auto view_comp = try_get<Transform>(m_camera);
	if (!view_comp) throw std::exception();
	const glm::mat4& view = glm::inverse(view_comp->getWorldMatrix());

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
	m_culling.setUniform1i("lightCount", storage<PointLight>().size());
	m_culling.dispatch(glm::uvec3(1, 1, m_clusterSize.z));
	
	m_program.bind();
	for (auto [entity, mesh, material, transform] : render_scene_view.each())
	{
		m_program.setUniformMat4("MVP", proj * view * transform.getWorldMatrix());
		material.bind(6);
		mesh.draw();
	}
	
#if defined(CLUSTER_DEBUG)
	glDisable(GL_DEPTH_TEST);

	m_debug_cluster_program.bind();
	m_debug_cluster_program.setUniformMat4("VP", proj * view);
	m_debug_cluster_vao.draw(GL_LINES, m_clusterSize.x * m_clusterSize.y * m_clusterSize.z);
	
	glEnable(GL_DEPTH_TEST);

	unsigned int visible_lights;
	m_visibleCountBuffer.get_data(&visible_lights);

	unsigned int lightCount = storage<PointLight>().size();

	std::vector<AABB> clusters { m_clusterSize.x * m_clusterSize.y * m_clusterSize.z };
	m_clusterBuffer.get_data(&clusters[0], sizeof(AABB) * m_clusterSize.x * m_clusterSize.y * m_clusterSize.z, 0);

	std::vector<PointLight> lights{ lightCount };
	m_pointLightBuffer.get_data(&lights[0], sizeof(PointLight) * storage<PointLight>().size(), 0);

	std::vector<int> indices = std::vector<int>(visible_lights, 0);
	m_lightIndiceBuffer.get_data(&indices[0], sizeof(int) * visible_lights, 0);

	std::vector<LightArray> lightArrays{ m_clusterSize.x * m_clusterSize.y * m_clusterSize.z };
	m_lightArrayBuffer.get_data(&lightArrays[0], sizeof(LightArray) * m_clusterSize.x * m_clusterSize.y * m_clusterSize.z, 0);

	std::sort(lightArrays.begin(), lightArrays.end(), 
		[](const LightArray& a, const LightArray& b) -> bool { 
			if (a.index == b.index) return a.count < b.count;
			else					return a.index < b.index;
		});
	
#endif
}

void RenderSystem::constructLight(GLbuffer& m_pointLightBuffer, entt::registry& reg, entt::entity e)
{
	if (reg.storage<PointLight>().size() == SCENE_MAX_LIGHTS) {
		// resize buffer???
		throw std::exception();
	}

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