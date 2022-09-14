#pragma once
#include "entt_macros.h"
#include "../Components/Projection.h"
#include "../Components/Transform.h"
#include "../Components/PointLight.h"
#include "../Components/Mesh.h"
#include "../Components/Material.h"
#include "../Rendering/Resources/ShaderProgram.h"
#include <glm.hpp>

#include <gtc/matrix_transform.hpp>

class RenderSystem : virtual protected entt::registry {
public:
	__declspec(property(get=getCamera,put=setCamera)) entt::entity camera;

	RenderSystem(glm::ivec2 size);

	void render();

	void resize(glm::ivec2 size);

	void setCamera(entt::entity e);

	entt::entity getCamera();

private:
	GROUP(render_view, OWN(Material), GET(VAO), EXC());
	VIEW(light_view, GET(PointLight), EXC());

	entt::entity m_camera;

	glm::uvec3 m_workGroups;

	GLbuffer m_ScreenBuffer;  // 0
	GLbuffer m_clusterBuffer; // 1
	GLbuffer m_ptLightBuffer; // 2
	GLbuffer m_visibleBuffer; // 3
	
	ShaderProgram<COMP> m_prepass{ "Resources/shaders/cluster_prepass.comp" };
	ShaderProgram<COMP> m_culling{ "Resources/shaders/cluster_culling.comp" };
	ShaderProgram<VERT, FRAG> m_shading{ "Resources/shaders/cluster_shading.shader" };

	ShaderProgram<VERT, FRAG> m_program{ "Resources/shaders/default.shader" };

	ShaderProgram<VERT, FRAG> m_line_program{ "Resources/shaders/debug_line.shader" };
	
	VAO m_line_vao;
};