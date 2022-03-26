#pragma once

#include <entt/entity/registry.hpp>
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"
#include "../Components/Camera.h"
#include "../Rendering/Resources/Model.h"
#include "../Rendering/Resources/ShaderProgram.h"

using entt::literals::operator""_hs;


// important to remember these are just for syntax and wont be enforced in the correct order
// It will always be OWN, GET, EXC
// Only group implements OWN
#define OWN(...) __VA_ARGS__
#define GET(...) __VA_ARGS__ 
#define EXC(...) __VA_ARGS__

#define TAG(TAG) using TAG = entt::tag<operator""_hs(#TAG, sizeof(#TAG))>;
#define VIEW(NAME, GET, EXC) entt::basic_view<entt::entity, entt::get_t<GET>, entt::exclude_t<EXC>, void> NAME = view<GET>(entt::exclude<EXC>);
#define GROUP(NAME, OWN, GET, EXC) entt::basic_group<entt::entity, entt::owned_t<OWN>, entt::get_t<GET>, entt::exclude_t<EXC>> NAME = group<OWN>(entt::get<GET>, entt::exclude<EXC>);

class Scene : private entt::basic_registry<entt::entity>
{
public:
	entt::entity camera;

public: // public selective inheritance of functions
	using entt::basic_registry<entt::entity>::get;
	using entt::basic_registry<entt::entity>::try_get;
	using entt::basic_registry<entt::entity>::create;
	using entt::basic_registry<entt::entity>::emplace;
	using entt::basic_registry<entt::entity>::erase;
	using entt::basic_registry<entt::entity>::destroy;
	using entt::basic_registry<entt::entity>::all_of;
	using entt::basic_registry<entt::entity>::any_of;

public:
	entt::entity load(std::string filepath);
	Scene();

	




private:
	VIEW(viewHierarchy, GET(Hierarchy), EXC())
	VIEW(viewTransform, GET(Transform), EXC())
	VIEW(viewRootTransform, GET(Transform), EXC(Hierarchy))


public:
	void SceneGraphUpdate();

private:
	VIEW(viewCamera, GET(Camera), EXC())
public:
	void UpdateCameraMatrix();

private:
	VIEW(viewRender, GET(Model), EXC(Transform /*, Skin*/))
	VIEW(viewRenderTransform, GET(Model, Transform), EXC())
	std::shared_ptr<ShaderProgram> defaultProgram; // 

public:
	void Render();
};

#undef OWN
#undef GET
#undef EXC
#undef TAG
#undef VIEW
#undef GROUP
