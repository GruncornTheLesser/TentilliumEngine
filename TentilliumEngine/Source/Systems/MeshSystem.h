#pragma once
#include "entt_macros.h"
#include "../Components/Mesh.h"
class MeshSystem : virtual protected entt::registry {
public:
	MeshSystem() {
		on_construct<Mesh::VBO<Mesh::V_Index>>().connect<attachVBO<Mesh::V_Index>>();
		on_construct<Mesh::VBO<Mesh::V_Position>>().connect<attachVBO<Mesh::V_Position>>();
		on_construct<Mesh::VBO<Mesh::V_TexCoord>>().connect<attachVBO<Mesh::V_TexCoord>>();
		on_construct<Mesh::VBO<Mesh::V_Normal>>().connect<attachVBO<Mesh::V_Normal>>();
		on_construct<Mesh::VBO<Mesh::V_BoneID>>().connect<attachVBO<Mesh::V_BoneID>>();
		on_construct<Mesh::VBO<Mesh::V_BoneWeight>>().connect<attachVBO<Mesh::V_BoneWeight>>();

		on_destroy<Mesh::VBO<Mesh::V_Index>>().connect<detachVBO<Mesh::V_Index>>();
		on_destroy<Mesh::VBO<Mesh::V_Position>>().connect<detachVBO<Mesh::V_Position>>();
		on_destroy<Mesh::VBO<Mesh::V_TexCoord>>().connect<detachVBO<Mesh::V_TexCoord>>();
		on_destroy<Mesh::VBO<Mesh::V_Normal>>().connect<detachVBO<Mesh::V_Normal>>();
		on_destroy<Mesh::VBO<Mesh::V_BoneID>>().connect<detachVBO<Mesh::V_BoneID>>();
		on_destroy<Mesh::VBO<Mesh::V_BoneWeight>>().connect<detachVBO<Mesh::V_BoneWeight>>();
	}

private:
	template<Mesh::VertAttrib attrib>
	static void attachVBO(entt::registry& reg, entt::entity e) {
		auto& vao = reg.get_or_emplace<Mesh::VAO>(e);
		vao.attach(reg.try_get<Mesh::VBO<attrib>>(e));
	}

	template<Mesh::VertAttrib attrib>
	static void detachVBO(entt::registry& reg, entt::entity e) {
		auto& vao = reg.get_or_emplace<Mesh::VAO>(e);
		vao.detach<attrib>();
	}
};