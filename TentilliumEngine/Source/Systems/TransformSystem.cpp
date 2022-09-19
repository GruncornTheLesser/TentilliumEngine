#include "TransformSystem.h"
#include <iostream>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/quaternion.hpp>


TransformSystem::TransformSystem()

{
	on_construct<Position>().connect<attachTransform>();
	on_construct<Scale>().connect<attachTransform>();
	on_construct<Rotation>().connect<attachTransform>();

	on_update<Position>().connect<updateTransform>();
	on_update<Scale>().connect<updateTransform>();
	on_update<Rotation>().connect<updateTransform>();

	on_destroy<Position>().connect<detachTransform>();
	on_destroy<Scale>().connect<detachTransform>();
	on_destroy<Rotation>().connect<detachTransform>();
}

void TransformSystem::update() {
	for (auto [e, local] : localUpdateView.each()) {

		auto scale = try_get<Scale>(e);
		auto rotation = try_get<Rotation>(e);
		auto position = try_get<Position>(e);

		if (!scale)		local = glm::mat4(); 
		else			local = scale->getMatrix();
		if (rotation)	local = rotation->getMatrix() * (glm::mat4)local;
		if (position)	local = position->getMatrix() * (glm::mat4)local;
	}

	for (auto [e, local, world] : worldRootUpdateView.each())
		world = WorldTransform((glm::mat4)local);

	for (auto [child, hierarchy, local, world] : worldUpdateView.each()) {
		WorldTransform* parent = try_get<WorldTransform>(hierarchy.parent);	// parent transform if exists
		if (parent && all_of<UpdateTag>(hierarchy.parent)) {				// if parent was updated
			world = (glm::mat4)(*parent) * (glm::mat4)local;				// update world Transform Tag
			get_or_emplace<UpdateTag>(child);
		}
		else if (all_of<UpdateTag>(child))									// if updated from local transform
			world = (glm::mat4)local;
	}
}

void TransformSystem::attachTransform(entt::registry& reg, entt::entity e)
{
	reg.get_or_emplace<LocalTransform>(e);
	reg.get_or_emplace<WorldTransform>(e);
	reg.get_or_emplace<UpdateTag>(e);
}

void TransformSystem::detachTransform(entt::registry& reg, entt::entity e)
{
	if (!reg.any_of<Position, Scale, Rotation>(e)) {
		reg.erase<LocalTransform>(e);
		reg.erase<WorldTransform>(e);
	}
	reg.get_or_emplace<UpdateTag>(e);
}

void TransformSystem::updateTransform(entt::registry& reg, entt::entity e)
{
	reg.get_or_emplace<UpdateTag>(e);
}
