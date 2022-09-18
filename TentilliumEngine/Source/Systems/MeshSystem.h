#pragma once
#include "entt_macros.h"
#include "../Components/Mesh.h"
class MeshSystem : virtual protected entt::registry {
public:
	MeshSystem() {
		on_construct<VBO<V_Index>>().connect<attachVBO<V_Index>>();
		on_construct<VBO<V_Position>>().connect<attachVBO<V_Position>>();
		on_construct<VBO<V_TexCoord>>().connect<attachVBO<V_TexCoord>>();
		on_construct<VBO<V_Normal>>().connect<attachVBO<V_Normal>>();
		on_construct<VBO<V_BoneID>>().connect<attachVBO<V_BoneID>>();
		on_construct<VBO<V_BoneWeight>>().connect<attachVBO<V_BoneWeight>>();

		on_destroy<VBO<V_Index>>().connect<detachVBO<V_Index>>();
		on_destroy<VBO<V_Position>>().connect<detachVBO<V_Position>>();
		on_destroy<VBO<V_TexCoord>>().connect<detachVBO<V_TexCoord>>();
		on_destroy<VBO<V_Normal>>().connect<detachVBO<V_Normal>>();
		on_destroy<VBO<V_BoneID>>().connect<detachVBO<V_BoneID>>();
		on_destroy<VBO<V_BoneWeight>>().connect<detachVBO<V_BoneWeight>>();
	}
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