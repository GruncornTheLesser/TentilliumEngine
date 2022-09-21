#pragma once
#include "entt_macros.h"
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"

class TransformSystem : virtual protected entt::registry {

public:
	TransformSystem();

	void update();

protected:
	

private:
	VIEW(localUpdateView, GET(Transform::LocalMatrix, Transform::UpdateTag), EXC());
	VIEW(worldUpdateView, GET(Parent, Transform::LocalMatrix, Transform::WorldMatrix), EXC());
	VIEW(rootUpdateView, GET(Transform::LocalMatrix, Transform::WorldMatrix, Transform::UpdateTag), EXC(Parent)); // any order

	static void attachTransform(entt::registry& reg, entt::entity e);

	static void detachTransform(entt::registry& reg, entt::entity e);

	static void updateTransform(entt::registry& reg, entt::entity e);
};