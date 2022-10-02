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
#define MAX_MATERIAL_COUNT 256

#define CALC_CLUSTER_SIZE(size) std::sqrtf(OPTIMUM_TILE_COUNT * size.x / size.y), \
								std::sqrtf(OPTIMUM_TILE_COUNT * size.y / size.x), \
								MAX_CLUSTER_COUNT / OPTIMUM_TILE_COUNT

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
	m_visibleCountBuffer(nullptr, sizeof(unsigned int)),
	m_materialBuffer(nullptr, sizeof(Material::UniformData) * MAX_MATERIAL_COUNT),
	m_screenVBO(std::vector<float>{ -1, 1, 0, -1, -1, 0, 1, 1, 0, 1, -1, 0 }),
	m_geomPosition(1, 1, Texture::Format::RGB_16F),
	m_geomDepth(1, 1, Texture::Format::DEPTH),
	m_geomNormal(1, 1, Texture::Format::RGB),
	m_geomTexCoord(1, 1, Texture::Format::RG),
	m_geomMaterial(1, 1, Texture::Format::R_32UI)
{
	// set point light events
	{
		on_construct<PointLight>().connect<constructLight>(m_pointLightBuffer);
		on_destroy<PointLight>().connect<destroyLight>(m_pointLightBuffer);
		on_update<PointLight>().connect<updateLight>(m_pointLightBuffer);
	}

	// set material events
	{
		on_construct<PointLight>().connect<constructLight>(m_pointLightBuffer);
		on_destroy<PointLight>().connect<destroyLight>(m_pointLightBuffer);
		on_update<PointLight>().connect<updateLight>(m_pointLightBuffer);
	}

	// set shader program uniforms
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_renderDataBuffer.handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_clusterBuffer.handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_pointLightBuffer.handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_lightIndiceBuffer.handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_lightArrayBuffer.handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_visibleCountBuffer.handle);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_materialBuffer.handle);

		m_deferredShadingProgram.setUniform("positionAttachment", 0); // bind to texture slot 0, 1 and 2
		m_deferredShadingProgram.setUniform("normalAttachment", 1);
		m_deferredShadingProgram.setUniform("texCoordAttachment", 2);
		m_deferredShadingProgram.setUniform("materialAttachment", 3);
		m_deferredShadingProgram.setUniform("depthAttachment", 4);

		m_deferredShadingProgram.setUniformBlock("MaterialBuffer", 0);
		m_geometryPassProgram.setUniformBlock("MaterialBuffer", 0);
		
	}
	
	// init framebuffer and screen mesh
	{
		// screen mesh
		m_screenVAO.attach(Mesh::V_Position, m_screenVBO, 3, GL_FLOAT, false, 0);

		// frame buffer
		glGenFramebuffers(1, &m_geometryBuffer);
		glGenRenderbuffers(1, &m_depthAttachment);

		glBindFramebuffer(GL_FRAMEBUFFER, m_geometryBuffer);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_geomPosition.handle, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_geomNormal.handle, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_geomTexCoord.handle, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_geomMaterial.handle, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_geomDepth.handle, 0);
		
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "[init error] - Framebuffer failed to initate" << std::endl;
			throw std::exception();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

RenderSystem::~RenderSystem() {
	glDeleteRenderbuffers(1, &m_depthAttachment);
	glDeleteFramebuffers(1, &m_geometryBuffer);
}

void RenderSystem::setSize(glm::ivec2 size)
{
	if (size.x == 0 || size.y == 0)  return;

	m_size = (glm::uvec2)size;
	m_clusterSize = glm::uvec3(CALC_CLUSTER_SIZE(size));

	m_geomPosition.setData(size.x, size.y, Texture::Format::RGB_16F);
	m_geomDepth.setData(size.x, size.y, Texture::Format::DEPTH);
	m_geomNormal.setData(size.x, size.y, Texture::Format::RGB);
	m_geomTexCoord.setData(size.x, size.y, Texture::Format::RG);
	m_geomMaterial.setData(size.x, size.y, Texture::Format::R_32UI);

	if (!valid(m_camera)) return;
	auto proj = try_get<Projection>(m_camera);
	if (!proj) return;

	proj->m_ratio = ((float)size.x) / size.y;
	proj->m_proj = glm::perspective(proj->m_fovY, proj->m_ratio, proj->m_zNear, proj->m_zFar);

	float scale = m_clusterSize.z / std::log2f(proj->m_zFar / proj->m_zNear);
	float bias = m_clusterSize.z * -std::log2f(proj->m_zNear) / std::log2f(proj->m_zFar / proj->m_zNear);

	m_renderDataBuffer.setData(RenderData{
		glm::inverse(proj->m_proj),
		m_clusterSize,
		bias,
		m_size,
		scale,
		proj->m_zNear,
		proj->m_zFar,		
		});

	m_clusterGenerationProgram.dispatch(m_clusterSize);
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

	m_renderDataBuffer.setData(RenderData{
		glm::inverse(proj.m_proj),
		m_clusterSize,
		bias,
		m_size,
		scale,
		proj.m_zNear,
		proj.m_zFar,
		});
	
	m_clusterGenerationProgram.dispatch(m_clusterSize);
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
	m_geometryPassProgram.setUniform("viewPosition", glm::vec3(view[3]));
	view = glm::inverse(view);

	// light culling
	m_lightCullingProgram.bind();
	m_lightCullingProgram.setUniform("view", view);
	m_lightCullingProgram.setUniform("lightCount", (int)storage<PointLight>().size());
	m_lightCullingProgram.dispatch(glm::uvec3(1, 1, m_clusterSize.z));

	// geometry prepass normal mapped
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_geometryBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_geometryPassProgram.bind();
	for (auto [entity, mesh, material, model] : render_scene_view.each()) 
	{
		m_geometryPassProgram.setUniform("model", model);
		m_geometryPassProgram.setUniform("MVP", proj * view * (glm::mat4)model);
		material.bind(0); // binds material maps 0 - 3
		mesh.draw();
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// deferred pass
	m_deferredShadingProgram.bind();
	m_geomPosition.bindSlot(0);
	m_geomNormal.bindSlot(1);
	m_geomTexCoord.bindSlot(2);
	m_geomMaterial.bindSlot(3);
	m_geomDepth.bindSlot(4);
	m_screenVAO.draw(GL_TRIANGLE_STRIP);

	// copy geometry depth buffer to main buffer
	// forward render transparent objects
}

void RenderSystem::constructLight(Buffer& buffer, entt::registry& reg, entt::entity e)
{
	if (reg.storage<PointLight>().size() == SCENE_MAX_LIGHTS)
		throw std::exception();

	// add light to end of buffer array
	auto back = reg.storage<PointLight>().size() - 1;

	buffer.setData(reg.get<PointLight>(e), back * sizeof(PointLight));
}

void RenderSystem::destroyLight(Buffer& buffer, entt::registry& reg, entt::entity e)
{
	size_t index = reg.storage<PointLight>().index(e);
	size_t back = reg.storage<PointLight>().size() - 1;
	if (index == back) return; 

	// remove light by swapping with last light
	buffer.setData(reg.get<PointLight>(reg.storage<PointLight>().at(back)), index * sizeof(PointLight));
}

void RenderSystem::updateLight(Buffer& buffer, entt::registry& reg, entt::entity e) {
	auto index = reg.storage<PointLight>().index(e);
	buffer.setData(reg.get<PointLight>(e).m_position, index * sizeof(PointLight) + offsetof(PointLight, m_position));
}

void RenderSystem::constructMaterial(Buffer& buffer, entt::registry& reg, entt::entity e)
{
	if (reg.storage<Material>().size() == MAX_MATERIAL_COUNT)
		throw std::exception();

	// add light to end of buffer array
	auto back = reg.storage<Material>().size() - 1;

	buffer.setData(reg.get<Material>(e), back * sizeof(Material));
}

void RenderSystem::destroyMaterial(Buffer& buffer, entt::registry& reg, entt::entity e)
{
	size_t index = reg.storage<Material>().index(e);
	size_t back = reg.storage<Material>().size() - 1;
	if (index == back) return;

	// remove light by swapping with last light
	buffer.setData(reg.get<Material>(reg.storage<Material>().at(back)), index * sizeof(Material));
}

