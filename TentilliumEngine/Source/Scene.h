#pragma once
#include <entt/entity/registry.hpp>
#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/HierarchySystem.h"
#include "Systems/LoadSystem.h"
#include "Systems/MeshSystem.h"

class Scene : HierarchySystem, TransformSystem, RenderSystem, MeshSystem, LoadSystem
{
public:
	// entt functions
	using entt::basic_registry<entt::entity>::create;
	using entt::basic_registry<entt::entity>::get;
	using entt::basic_registry<entt::entity>::erase;
	using entt::basic_registry<entt::entity>::valid;
	using entt::basic_registry<entt::entity>::destroy;
	using entt::basic_registry<entt::entity>::all_of;
	using entt::basic_registry<entt::entity>::any_of;
	using entt::basic_registry<entt::entity>::clear;
	// aliased entt functions
	template<typename Component, typename ... Args>
	void set(entt::entity e, Args&& ... args) {
		emplace_or_replace<Component>(e, args...);
	}


	// render functions
	using RenderSystem::size;
	using RenderSystem::setSize;
	using RenderSystem::getSize;
	using RenderSystem::camera;
	using RenderSystem::setCamera;
	using RenderSystem::getCamera;
	using RenderSystem::render;

	// load functions
	using LoadSystem::load;
	
	void process() {
		HierarchySystem::update();
		TransformSystem::update();
		RenderSystem::render();

		clear<Transform::UpdateTag>();
	}
};
