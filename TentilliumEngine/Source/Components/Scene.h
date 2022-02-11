#pragma once
#include <entt/entity/registry.hpp>
#include "Hierarchy.h"
#include "Transform.h"
#include <entt/entt.hpp>

using entt::literals::operator""_hs;

class Scene : protected entt::basic_registry<entt::entity>
{
private:
	

	using transform_update_local_tag = entt::tag<"transform_update_local"_hs>;
	using transform_update_world_tag = entt::tag<"transform_update_world"_hs>;
	using hierarchy_update_depth_tag = entt::tag<"hierarchy_update_depth"_hs>;

	entt::basic_group<entt::entity, entt::owned_t<Transform, Hierarchy>, entt::get_t<>, entt::exclude_t<>> groupSceneGraph						= group<Transform, Hierarchy>();

	entt::basic_view<entt::entity, entt::get_t<Hierarchy>, entt::exclude_t<>, void> viewHierarchy												= view<Hierarchy>();
	entt::basic_view<entt::entity, entt::get_t<Transform>, entt::exclude_t<>, void> viewTransform												= view<Transform>();
	entt::basic_view<entt::entity, entt::get_t<Transform, transform_update_local_tag>, entt::exclude_t<>, void> viewLocalTransformUpdate		= view<Transform, transform_update_local_tag>();
	entt::basic_view<entt::entity, entt::get_t<Transform, transform_update_world_tag>, entt::exclude_t<Hierarchy>, void> viewRootTransform		= view<Transform, transform_update_world_tag>(entt::exclude<Hierarchy>);
	entt::basic_view<entt::entity, entt::get_t<Transform, Hierarchy, transform_update_world_tag>, entt::exclude_t<>, void> viewBranchTransform	= view<Transform, Hierarchy, transform_update_world_tag>();

public:
	template<class ... Components>
	entt::entity CreateEntity(Components ... components)
	{
		auto e = create();
		((void)emplace<Components>(e, components), ...);
		return e;
	}
	
	template<class ... Components>
	void AddComponents(entt::entity e, Components ... components)
	{
		((void)emplace<Components>(e, components), ...);
	}

	// systems
	void TagUpdate();
	void Calculate();

};

