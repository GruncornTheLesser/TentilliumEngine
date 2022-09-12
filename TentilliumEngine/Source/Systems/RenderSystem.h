#pragma once
#include "entt_macros.h"
#include "../Components/Projection.h"
#include "../Components/Transform.h"
#include "../Components/Mesh.h"
#include "../Components/Material.h"
#include "../Components/PointLight.h"
#include "../Rendering/Resources/ShaderProgram.h"
#include "../Rendering/Resources/Framebuffer.h"
#include <glm.hpp>

#include <gtc/matrix_transform.hpp>

class RenderSystem : virtual protected entt::registry {
private:
	GROUP(render_view, OWN(Material), GET(VAO), EXC());
	VIEW(light_view, GET(PointLight), EXC());

	ShaderProgram m_program{ "Resources/shaders/default.shader" };

	ShaderProgram m_depth_prepass{ "Resources/shaders/depth_prepass.shader" };
	ShaderProgram m_light_culling{ "Resources/shaders/light_culling.comp" };
	ShaderProgram m_light_accumul{ "Resources/shaders/light_accumul.shader" };

	Framebuffer m_framebuffer;

	GLbuffer m_lightBuffer;
	GLbuffer m_visibleLightBuffer;
	glm::ivec2 workGroups;
public:
	entt::entity camera;

public:
	RenderSystem();

	void resize(int width, int height);

	void render();

private:
	void calculateTiles(unsigned int width, unsigned int height);

	void updateLight();
};