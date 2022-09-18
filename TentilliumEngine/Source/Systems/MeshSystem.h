#pragma once
#include "entt_macros.h"
#include "../Components/Mesh.h"
class MeshSystem : virtual protected entt::registry {
public:
	MeshSystem() {
		on_construct<VBO<Index>>().connect<attachVBO<Index>>();
		on_construct<VBO<Position>>().connect<attachVBO<Position>>();
		on_construct<VBO<TexCoord>>().connect<attachVBO<TexCoord>>();
		on_construct<VBO<Normal>>().connect<attachVBO<Normal>>();
		on_construct<VBO<BoneID>>().connect<attachVBO<BoneID>>();
		on_construct<VBO<BoneWeight>>().connect<attachVBO<BoneWeight>>();

		on_destroy<VBO<Index>>().connect<detachVBO<Index>>();
		on_destroy<VBO<Position>>().connect<detachVBO<Position>>();
		on_destroy<VBO<TexCoord>>().connect<detachVBO<TexCoord>>();
		on_destroy<VBO<Normal>>().connect<detachVBO<Normal>>();
		on_destroy<VBO<BoneID>>().connect<detachVBO<BoneID>>();
		on_destroy<VBO<BoneWeight>>().connect<detachVBO<BoneWeight>>();
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