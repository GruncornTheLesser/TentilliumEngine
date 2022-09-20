#include "TransformSystem.h"
#include <iostream>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/quaternion.hpp>


TransformSystem::TransformSystem()

{
	on_construct<Transform::Position>().connect<attachTransform>();
	on_construct<Transform::Scale>().connect<attachTransform>();
	on_construct<Transform::Rotation>().connect<attachTransform>();

	on_update<Transform::Position>().connect<updateTransform>();
	on_update<Transform::Scale>().connect<updateTransform>();
	on_update<Transform::Rotation>().connect<updateTransform>();

	on_destroy<Transform::Position>().connect<detachTransform>();
	on_destroy<Transform::Scale>().connect<detachTransform>();
	on_destroy<Transform::Rotation>().connect<detachTransform>();
}

void TransformSystem::update() {
	for (auto [e, local] : localUpdateView.each()) {

		auto scale = try_get<Transform::Scale>(e);
		auto rotation = try_get<Transform::Rotation>(e);
		auto position = try_get<Transform::Position>(e);

		if (!scale)		local = glm::mat4(); 
		else			local = scale->getMatrix();
		if (rotation)	local = rotation->getMatrix() * (glm::mat4)local;
		if (position)	local = position->getMatrix() * (glm::mat4)local;
	}

	for (auto [e, local, world] : worldRootUpdateView.each())
		world = Transform::WorldMatrix((glm::mat4)local);

	for (auto [child, hierarchy, local, world] : worldUpdateView.each()) {
		Transform::WorldMatrix* parent = try_get<Transform::WorldMatrix>(hierarchy.parent);	// parent transform if exists
		if (parent && all_of<Transform::UpdateTag>(hierarchy.parent)) {	// if parent was updated
			world = (glm::mat4)(*parent) * (glm::mat4)local;			// update world Transform Tag
			get_or_emplace<Transform::UpdateTag>(child);
		}
		else if (all_of<Transform::UpdateTag>(child))					// if updated from local transform
			world = (glm::mat4)local;
	}
}

void TransformSystem::attachTransform(entt::registry& reg, entt::entity e)
{
	if (!reg.any_of<Transform::LocalMatrix>(e)) reg.emplace<Transform::LocalMatrix>(e);
	if (!reg.any_of<Transform::WorldMatrix>(e)) reg.emplace<Transform::WorldMatrix>(e);
	
	if (!reg.any_of<Transform::UpdateTag>(e)) reg.emplace<Transform::UpdateTag>(e);
}

void TransformSystem::detachTransform(entt::registry& reg, entt::entity e)
{
	if (!reg.any_of<Transform::Position, Transform::Scale, Transform::Rotation>(e)) {
		reg.erase<Transform::LocalMatrix>(e);
		reg.erase<Transform::WorldMatrix>(e);
	}
}

void TransformSystem::updateTransform(entt::registry& reg, entt::entity e)
{
	if (!reg.any_of<Transform::UpdateTag>(e)) reg.emplace<Transform::UpdateTag>(e);
}
