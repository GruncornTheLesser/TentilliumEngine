#pragma once
#include "entt_macros.h"
#include "../Components/Projection.h"
#include "../Components/Transform.h"
#include "../Components/Mesh.h"
#include "../Components/SpecularMaterial.h"
#include "../Rendering/Resources/ShaderProgram.h"
#include <glm.hpp>

#include <gtc/matrix_transform.hpp>

class RenderSystem : virtual protected entt::registry {
public:
	entt::entity camera;
	glm::mat4 get_cam_view();
	glm::mat4 get_cam_proj();
	void resize(int width, int height);

private:
	GROUP(specular_view, OWN(SpecularMaterial), GET(VAO), EXC());
	ShaderProgram m_program{ "Resources/shaders/default.shader" };

	ShaderProgram m_depth_prepass{ "Resources/shaders/depth_prepass.shader" };
	ShaderProgram m_light_culling{ "Resources/shaders/light_culling.comp" };
	ShaderProgram m_light_accumul{ "Resources/shaders/light_accumul.shader" };
public:
	RenderSystem();

	void render();
};