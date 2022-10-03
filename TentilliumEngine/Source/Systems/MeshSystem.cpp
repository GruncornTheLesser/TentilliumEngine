#include "MeshSystem.h"
#include "glew.h"
#include "glfw3.h"

using namespace Mesh;

template<Mesh::VertAttrib attrib>
void MeshSystem::detachVBO(entt::registry& reg, entt::entity e)
{
	reg.get<VAO>(e).detach(attrib);

	if (!reg.any_of<VBO<V_Index>, VBO<V_Position>, VBO<V_Normal>, VBO<V_Tangent>, 
		VBO<V_TexCoord>, VBO<V_BoneID>, VBO<V_BoneWeight>, VBO<V_MaterialIndex>>(e))
	{
		reg.erase<VAO>(e);
	}
}

template<> void MeshSystem::attachVBO<V_Index>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_Index, reg.get<VBO<V_Index>>(e), 1, GL_UNSIGNED_INT, false, 0);
}

template<> void MeshSystem::attachVBO<V_Position>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_Position, reg.get<VBO<V_Position>>(e), 3, GL_FLOAT, false, 0);
}

template<> void MeshSystem::attachVBO<V_Normal>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_Normal, reg.get<VBO<V_Normal>>(e), 3, GL_FLOAT, false, 0);
}

template<> void MeshSystem::attachVBO<V_Tangent>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_Tangent, reg.get<VBO<V_Tangent>>(e), 3, GL_FLOAT, false, 0);
}

template<> void MeshSystem::attachVBO<V_TexCoord>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_TexCoord, reg.get<VBO<V_TexCoord>>(e), 2, GL_FLOAT, false, 0);
}

template<> void MeshSystem::attachVBO<V_BoneID>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_BoneID, reg.get<VBO<V_BoneID>>(e), 4, GL_INT, false, 0);
}

template<> void MeshSystem::attachVBO<V_BoneWeight>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_BoneWeight, reg.get<VBO<V_BoneWeight>>(e), 4, GL_FLOAT, true, 0);
}

template<> void MeshSystem::attachVBO<V_MaterialIndex>(entt::registry& reg, entt::entity e) {
	reg.get_or_emplace<VAO>(e).attach(V_MaterialIndex, reg.get<VBO<V_MaterialIndex>>(e), 1, GL_FLOAT, false, 0); // why does float work???
}

MeshSystem::MeshSystem()
{
	on_construct<VBO<V_Index>>().connect<attachVBO<V_Index>>();
	on_construct<VBO<V_Position>>().connect<attachVBO<V_Position>>();
	on_construct<VBO<V_Normal>>().connect<attachVBO<V_Normal>>();
	on_construct<VBO<V_Tangent>>().connect<attachVBO<V_Tangent>>();
	on_construct<VBO<V_TexCoord>>().connect<attachVBO<V_TexCoord>>();
	on_construct<VBO<V_BoneID>>().connect<attachVBO<V_BoneID>>();
	on_construct<VBO<V_BoneWeight>>().connect<attachVBO<V_BoneWeight>>();
	on_construct<VBO<V_MaterialIndex>>().connect<attachVBO<V_MaterialIndex>>();

	on_destroy<VBO<V_Index>>().connect<detachVBO<V_Index>>();
	on_destroy<VBO<V_Position>>().connect<detachVBO<V_Position>>();
	on_destroy<VBO<V_Normal>>().connect<detachVBO<V_Normal>>();
	on_destroy<VBO<V_Tangent>>().connect<detachVBO<V_Tangent>>();
	on_destroy<VBO<V_TexCoord>>().connect<detachVBO<V_TexCoord>>();
	on_destroy<VBO<V_BoneID>>().connect<detachVBO<V_BoneID>>();
	on_destroy<VBO<V_BoneWeight>>().connect<detachVBO<V_BoneWeight>>();
	on_destroy<VBO<V_MaterialIndex>>().connect<detachVBO<V_MaterialIndex>>();
}