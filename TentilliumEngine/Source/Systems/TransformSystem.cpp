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
	using namespace Transform;
	for (auto [e, local] : localUpdateView.each()) {

		if (auto scale = try_get<Scale>(e))			local = scale->getMatrix();
		else										local = glm::mat4(); 
		if (auto rotation = try_get<Rotation>(e))	local = rotation->getMatrix() * (glm::mat4)local;
		if (auto position = try_get<Position>(e))	local = position->getMatrix() * (glm::mat4)local;
	}

	for (auto [e, local, world] : rootUpdateView.each())
		world = WorldMatrix((glm::mat4)local);

	for (auto [child, parent, local, world] : worldUpdateView.each()) {
		if (all_of<UpdateTag>(parent))
		{
			if (!any_of<UpdateTag>(child)) // propergate UpdateTag
				emplace<UpdateTag>(child);
			
			// contains UpdateTag implies contains Transform Components
		}
		else if (all_of<UpdateTag>(child))
		{
			if (!all_of<WorldMatrix>(parent))	// if no parent Transform
			{
				world = (glm::mat4)local;		// treat as root transform
				continue;						// skip parent update path
			}

			// Update Tag already added -> no need to propergate Update Tag
		}
		else continue; // neither child nor parent Transform update, continue

		// update from parent
		glm::mat4 parentTransform = get<WorldMatrix>(parent);
		world = parentTransform * (glm::mat4)local;
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
