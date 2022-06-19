#pragma once
#include <entt/entity/registry.hpp>
#include "Systems/TransformSystem.h"
#include "Systems/SpecularSystem.h"
#include "Systems/LoadSystem.h"

class Scene : TransformSystem, SpecularSystem, LoadSystem
{
public:
	using entt::basic_registry<entt::entity>::get;
	using entt::basic_registry<entt::entity>::get_or_emplace;
	using entt::basic_registry<entt::entity>::try_get;
	using entt::basic_registry<entt::entity>::create;
	using entt::basic_registry<entt::entity>::emplace;
	using entt::basic_registry<entt::entity>::erase;
	using entt::basic_registry<entt::entity>::destroy;
	using entt::basic_registry<entt::entity>::all_of;
	using entt::basic_registry<entt::entity>::any_of;
	using entt::basic_registry<entt::entity>::view;
	using entt::basic_registry<entt::entity>::group;

	using TransformSystem::update;
	using SpecularSystem::render;

	using RenderSystem::resize;
	using RenderSystem::set_camera;
	
	using LoadSystem::load;
	

	void process() {
		TransformSystem::update();
		SpecularSystem::render();
	}
};
