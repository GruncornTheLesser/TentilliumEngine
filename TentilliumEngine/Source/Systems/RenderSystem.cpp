#include "RenderSystem.h"
#include "../Components/Mesh.h"
#include <glew.h>
#include <glfw3.h>

#define MAX_LIGHT 4

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

RenderSystem::RenderSystem()
{
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

	m_program.setUniform1i("map", 0);
	m_light_culling.setUniform1i("depthMap", 4);

	m_light_accumul.setUniform1i("diffuseMap", 1);
	m_light_accumul.setUniform1i("specularMap", 2);
	m_light_accumul.setUniform1i("normalMap", 3);

	glNamedFramebufferDrawBuffer(m_framebuffer.handle, GL_NONE);
	glNamedFramebufferReadBuffer(m_framebuffer.handle, GL_NONE);
}

void RenderSystem::calculateTiles(unsigned int width, unsigned int height) {
	workGroups.x = (width  + (width  % 16)) / 16;
	workGroups.y = (height + (height % 16)) / 16;

	m_light_culling.setUniform2i("screenSize", glm::ivec2(width, height));
	m_light_culling.setUniform1i("lightCount", (unsigned int)light_view.size());

	m_light_accumul.setUniform1i("numberOfTilesX", workGroups.x);

	m_visibleLightBuffer = GLbuffer(nullptr, workGroups.x * workGroups.y * sizeof(unsigned int) * MAX_LIGHT, GL_DYNAMIC_DRAW);
}

void RenderSystem::updateLight()
{
	m_lightBuffer = GLbuffer(nullptr, light_view.size() * sizeof(PointLight));

	PointLight* pointlights = (PointLight*)glMapNamedBuffer(m_lightBuffer.handle, GL_READ_WRITE);
	
	unsigned int i = 0;
	for (auto [entity, light] : light_view.each()) 
		pointlights[i++] = light;

	glUnmapNamedBuffer(m_lightBuffer.handle);
}

void RenderSystem::resize(int width, int height)
{
	if (width == 0 || height == 0) return;

	if (auto ptr = try_get<Projection>(camera))
		ptr->m_proj = glm::perspective(ptr->m_fovY, ((float)width) / height, ptr->m_zNear, ptr->m_zFar);

	m_framebuffer.detach(GL_DEPTH_ATTACHMENT);
	m_framebuffer.attach(GL_DEPTH_ATTACHMENT, Texture(nullptr, width, height, Texture::Format::DEPTH));

	calculateTiles(width, height);

	updateLight();
}

void RenderSystem::render()
{
	// get camera
	if (!valid(camera)) return;
	// get camera projection
	auto proj_comp = try_get<Projection>(camera);
	if (!proj_comp) return;
	glm::mat4 proj = proj_comp->get();
	// get camera view
	auto view_comp = try_get<Transform>(camera);
	if (!view_comp) return;
	glm::mat4 view = glm::inverse(view_comp->getWorldMatrix());
	
	// Step 1
	// render depth of the scene to a depth map
	m_depth_prepass.bind();
	// bind depth frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.handle);
	glClear(GL_DEPTH_BUFFER_BIT);
	// draw scene to depth buffer
	for (auto [entity, material, vao] : render_view.each()) {
		if (auto transform = try_get<Transform>(entity))
			m_depth_prepass.setUniformMat4("MVP", proj * view * transform->getWorldMatrix());
		vao.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//Step 2
	// Perform light culling on point lights in the scene
	m_light_culling.bind();
	m_light_culling.setUniformMat4("view", view);
	m_light_culling.setUniformMat4("proj", proj);

	// bind depth texture to slot 4
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer.get(GL_DEPTH_ATTACHMENT).handle);
	// bind shader buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightBuffer.handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_visibleLightBuffer.handle);
	// Dispatch the compute shader, using the workgroup values calculated earlier
	glDispatchCompute(workGroups.x, workGroups.y, 1);
	// unbind clear texture slot 4
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::vector<PointLight> data1(light_view.size());
	m_lightBuffer.get_data(&data1[0], light_view.size() * sizeof(PointLight), 0);

	std::vector<int> data2(workGroups.x * workGroups.y * MAX_LIGHT);
	m_visibleLightBuffer.get_data(&data2[0], workGroups.x * workGroups.y * MAX_LIGHT * sizeof(int), 0);

	// Step 3
	// Accumulate the remaining lights after culling and render 
	m_light_accumul.bind();
	m_light_accumul.setUniform3f("viewPosition", glm::vec3(view[3]));
	// draw to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto [entity, material, vao] : render_view.each())
	{
		if (auto transform = try_get<Transform>(entity)) {
			m_light_accumul.setUniformMat4("model", transform->getWorldMatrix());
			m_light_accumul.setUniformMat4("MVP", proj * view * transform->getWorldMatrix());
		}

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, material.handle);
		for (auto [slot, texture] : material.m_textures)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, texture.handle);
		}

		vao.draw();
	}

	/*
	m_program.bind();
	for (auto [entity, material, vao] : render_view.each())
	{
		if (auto transform = try_get<Transform>(entity))
			m_program.setUniformMat4("MVP", proj * view * transform->getWorldMatrix());

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, material.handle);

		for (auto [slot, texture] : material.m_textures)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, texture.handle);
		}

		vao.draw();
	}
	*/
}

