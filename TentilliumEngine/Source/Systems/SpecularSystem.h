#pragma once
#include "RenderSystem.h"
#include "../Components/Mesh.h"
#include "../Components/SpecularMaterial.h"
#include "../Rendering/Resources/ShaderProgram.h"


class SpecularSystem : virtual protected RenderSystem {
private:
	GROUP(specular_view, OWN(SpecularMaterial), GET(VAO), EXC());

	ShaderProgram m_program{ "Resources/shaders/default.shader" };

public:
	SpecularSystem();
	
protected:
	void render();

private:
	static void initVAO(entt::registry& reg, entt::entity e);
};