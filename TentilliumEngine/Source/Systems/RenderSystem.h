#pragma once
#include "entt_macros.h"
#include "../Components/Projection.h"
#include "../Components/Transform.h"
#include "../Components/PointLight.h"
#include "../Components/Mesh.h"
#include "../Components/Material.h"
#include "../Rendering/Resources/ShaderProgram.h"
#include "../Rendering/Resources/Framebuffer.h"
#include <glm.hpp>

//#define CLUSTER_DEBUG

class RenderSystem : virtual protected entt::registry {
public:
	__declspec(property(get = getSize, put = setSize)) glm::ivec2 size;
	__declspec(property(get=getCamera,put=setCamera)) entt::entity camera;
	
	RenderSystem();

	void render();

	void setSize(glm::ivec2 size);

	glm::ivec2 getSize();

	void setCamera(entt::entity e);

	entt::entity getCamera();
private:
	VIEW(render_scene_view, GET(VAO, Material, Transform::WorldMatrix), EXC());
	VIEW(light_view, GET(PointLight), EXC());

	entt::entity m_camera;

	glm::uvec2 m_size;

	glm::uvec3 m_clusterSize;

	// shared storage buffer objects
	GLbuffer m_renderDataBuffer;	// 0
	GLbuffer m_clusterBuffer;		// 1
	GLbuffer m_pointLightBuffer;	// 2
	GLbuffer m_lightIndiceBuffer;	// 3
	GLbuffer m_lightArrayBuffer;	// 4
	GLbuffer m_visibleCountBuffer;	// 5

	Framebuffer m_depth_framebuffer;

	ShaderProgram<COMP> m_prepass{ "Resources/shaders/cluster_prepass.comp" };
	ShaderProgram<COMP> m_culling{ "Resources/shaders/cluster_culling.comp" };
	ShaderProgram<VERT, FRAG> m_shading{ "Resources/shaders/cluster_shading.shader" };

	//ShaderProgram<VERT, FRAG> m_depth_prepass{ "Resources/shaders/depth_prepass.shader" };

#if defined(CLUSTER_DEBUG)
	ShaderProgram<VERT, GEOM, FRAG> m_debug_cluster_program{ "Resources/shaders/debug_cluster.shader" };
	VAO m_debug_cluster_vao;
#endif
	static void constructLight(GLbuffer& buffer, entt::registry& reg, entt::entity e);

	static void destroyLight(GLbuffer& buffer, entt::registry& reg, entt::entity e);

	static void updateLight(GLbuffer& buffer, entt::registry& reg, entt::entity e);
};