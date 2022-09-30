#pragma once
#include "entt_macros.h"
#include "../Components/Mesh.h"
class MeshSystem : virtual protected entt::registry {
public:
	MeshSystem();

private:
	template<Mesh::VertAttrib attrib>
	static void attachVBO(entt::registry& reg, entt::entity e);

	template<Mesh::VertAttrib attrib>
	static void detachVBO(entt::registry& reg, entt::entity e);
};