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

	m_deferredShadingProgram.setUniform1i("fboAttachment0", 0); // bind to texture slot 0, 1 and 2
	m_deferredShadingProgram.setUniform1i("fboAttachment1",	1);
	m_deferredShadingProgram.setUniform1i("fboAttachment2", 2);

	m_geometryPassProgram.setUniformBlock("Material", 0);

	m_geometryPassProgram.setUniform1i("diffuseMap", 0); // bind to material textures to slot 0, 1, 2 and 3
	m_geometryPassProgram.setUniform1i("specularMap", 1);
	m_geometryPassProgram.setUniform1i("glossMap", 2);
	m_geometryPassProgram.setUniform1i("normalMap", 3);
}

void RenderSystem::setSize(glm::ivec2 size)
{
	if (size.x == 0 || size.y == 0)  return;

	m_size = (glm::uvec2)size;
	m_clusterSize = glm::uvec3(CALC_CLUSTER_SIZE(size));

	m_geometryBuffer.resize(size.x, size.y);

	if (!valid(m_camera)) return;
	auto proj = try_get<Projection>(m_camera);
	if (!proj) return;

	proj->m_ratio = ((float)size.x) / size.y;
	proj->m_proj = glm::perspective(proj->m_fovY, proj->m_ratio, proj->m_zNear, proj->m_zFar);

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

	m_renderDataBuffer.set_data(RenderData{
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
	view = glm::inverse(view);

	// geometry prepass
	m_geometryBuffer.DrawTo();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_geometryPassProgram.bind();
	for (auto [entity, mesh, material, model] : render_scene_view.each()) {
		m_geometryPassProgram.setUniformMat4("model", model);
		m_geometryPassProgram.setUniformMat4("MVP", proj * view * (glm::mat4)model);
		material.bind(0); // binds material maps 0 - 3
		mesh.draw();
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// light culling
	m_lightCullingProgram.bind();
	m_lightCullingProgram.setUniformMat4("view", view);
	m_lightCullingProgram.setUniform1i("lightCount", storage<PointLight>().size());
	m_lightCullingProgram.dispatch(glm::uvec3(1, 1, m_clusterSize.z));
	

	// deferred pass
	m_deferredShadingProgram.bind();
	m_geometryBuffer.m_colourAttachment0.bindSlot(0); // binds geometry maps 0 - 2
	m_geometryBuffer.m_colourAttachment1.bindSlot(1);
	m_geometryBuffer.m_colourAttachment2.bindSlot(2);
	m_screenMesh.draw(GL_TRIANGLE_STRIP);

	// copy geometry depth buffer to main buffer
	// forward render transparent objects
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



RenderSystem::GeometryBuffer::GeometryBuffer()
{
	glGenFramebuffers(1, &m_framebuffer);
	glGenRenderbuffers(1, &m_depthAttachment);

	m_colourAttachment0.setData(1, 1, Texture::Format::RGBA, false); // *important* NOT normalized
	m_colourAttachment1.setData(1, 1, Texture::Format::RGBA, false);
	m_colourAttachment2.setData(1, 1, Texture::Format::RGBA, false);
	
	m_colourAttachment0.filter = Texture::Filter::NEAREST;	// position + depth
	m_colourAttachment1.filter = Texture::Filter::LINEAR;	// normal + 
	m_colourAttachment2.filter = Texture::Filter::LINEAR;

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

	// glBindTexture(GL_TEXTURE_2D, m_colourAttachment0.handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colourAttachment0.handle, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_colourAttachment1.handle, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_colourAttachment2.handle, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthAttachment);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1, 1);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthAttachment);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderSystem::GeometryBuffer::~GeometryBuffer()
{
	glDeleteRenderbuffers(1, &m_depthAttachment);
	glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderSystem::GeometryBuffer::resize(int width, int height)
{
	m_colourAttachment0.setData(width, height, Texture::Format::RGBA, false);
	m_colourAttachment1.setData(width, height, Texture::Format::RGBA, false);
	m_colourAttachment2.setData(width, height, Texture::Format::RGBA, false);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthAttachment);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void RenderSystem::GeometryBuffer::DrawTo() const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
}

void RenderSystem::GeometryBuffer::copyDepth(const glm::ivec2& size) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
	glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}



RenderSystem::ScreenMesh::ScreenMesh()
	: m_vbo(std::vector<float>{ -1, 1, 0, -1, -1, 0, 1, 1, 0, 1, -1, 0 })
{
	attach(&m_vbo);
}
