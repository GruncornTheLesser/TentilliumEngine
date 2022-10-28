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

class RenderSystem : virtual protected entt::registry {
public:
	__declspec(property(get=getSize, put=setSize)) glm::ivec2 size;
	__declspec(property(get=getCamera,put=setCamera)) entt::entity camera;
	
	RenderSystem();

	void render();

	void setSize(glm::ivec2 size);

	glm::ivec2 getSize();

	void setCamera(entt::entity e);

	entt::entity getCamera();

private:
	VIEW(render_scene_view, GET(Mesh::VAO, Material, Transform::WorldMatrix), EXC());
	VIEW(light_view, GET(PointLight), EXC());

	entt::entity m_camera = entt::tombstone;

	glm::uvec2 m_size;

	glm::uvec3 m_clusterSize;

	// shared storage buffer objects
	Buffer m_renderDataBuffer;		// stores basic data such as current screen size used in the render cycle
	Buffer m_clusterBuffer;			// stores an unsorted collection of Axis Aligned Bounding Boxes for each cluster
	Buffer m_pointLightBuffer;		// stores the point lights
	Buffer m_lightIndiceBuffer;		// stores indices to the pointlights
	Buffer m_lightArrayBuffer;		// stores an array per cluster of indices
	Buffer m_visibleCountBuffer;	// stores an unsigned int counting the number of lights in the scene(counting repeats)

	ShaderProgram<COMP>		  m_clusterGenerationProgram{ "Resources/shaders/cluster_prepass.comp" };	// calculates cluster AABBs
	ShaderProgram<COMP>		  m_lightCullingProgram{ "Resources/shaders/cluster_culling.comp" };		// culls lights from clusters
	ShaderProgram<VERT, FRAG> m_geometryPassProgram{ "Resources/shaders/geometry_pass.shader" };		// writes scene to geometry buffer
	ShaderProgram<VERT, FRAG> m_deferredShadingProgram{ "Resources/shaders/deferred_shading.shader" };	// shades scene

	Framebuffer m_geometryBuffer;

	static void constructLight(Buffer& buffer, entt::registry& reg, entt::entity e);

	static void destroyLight(Buffer& buffer, entt::registry& reg, entt::entity e);

	static void updateLight(Buffer& buffer, entt::registry& reg, entt::entity e);
};