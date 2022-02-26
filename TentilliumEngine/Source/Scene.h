#pragma once
#include <entt/entity/registry.hpp>
#include "Components/Hierarchy.h"
#include "Components/Transform.h"
#include "Components/Camera.h"


using entt::literals::operator""_hs;


#define OWN(...) __VA_ARGS__
#define GET(...) __VA_ARGS__
#define EXC(...) __VA_ARGS__

#define TAG(TAG) using TAG = entt::tag<operator""_hs(#TAG, sizeof(#TAG))>;
#define VIEW(NAME, GET, EXC) entt::basic_view<entt::entity, entt::get_t<GET>, entt::exclude_t<EXC>, void> NAME = view<GET>(entt::exclude<EXC>);
#define GROUP(NAME, OWN, GET, EXC) entt::basic_group<entt::entity, entt::owned_t<OWN>, entt::get_t<GET>, entt::exclude_t<EXC>> NAME = group<OWN>(entt::get<GET>, entt::exclude<EXC>);

class Scene : private entt::basic_registry<entt::entity>
{
private:
	unsigned int currentVersion = 0;

public:
	Scene() {}

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

	// hierarchy/transform system
private:
	GROUP(groupSceneGraph, OWN(Transform, Hierarchy), GET(), EXC())
	VIEW(viewTransform, GET(Transform), EXC())
	VIEW(viewHierarchy, GET(Hierarchy), EXC())
	VIEW(viewRootTransform, GET(Transform), EXC(Hierarchy))

	VIEW(viewCamera, GET(Camera), EXC())
public:

	void HierarchyUpdate();
	void TransformUpdate();

};

#undef OWN
#undef GET
#undef EXC
#undef TAG
#undef VIEW
#undef GROUP