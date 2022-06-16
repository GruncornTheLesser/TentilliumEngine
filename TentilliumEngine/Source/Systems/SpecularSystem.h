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


};