#pragma once
#include "RenderSystem.h"
#include "../Rendering/Resources/Mesh.h"
#include "../Components/SpecularMaterial.h"
#include "../Rendering/Resources/ShaderProgram.h"


class SpecularSystem : virtual protected RenderSystem {
private:
	VIEW(specular_view, GET(SpecularMaterial), EXC());
	ShaderProgram m_program{ "Resources/shaders/default.shader" };

public:
	SpecularSystem();
	
protected:
	void render();

private:
	static void initVAO(entt::registry& reg, entt::entity e);
};