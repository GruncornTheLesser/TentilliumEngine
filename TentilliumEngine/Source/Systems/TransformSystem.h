#pragma once
#include "entt_macros.h"
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"

class TransformSystem : virtual protected entt::registry {

public:
	TransformSystem();

	void update();

protected:
	TAG(UpdateTag);

private:
	VIEW(localUpdateView, GET(LocalTransform, UpdateTag), EXC());
	VIEW(worldUpdateView, GET(Hierarchy, LocalTransform, WorldTransform), EXC());
	VIEW(worldRootUpdateView, GET(LocalTransform, WorldTransform), EXC(Hierarchy)); // any order

	static void attachTransform(entt::registry& reg, entt::entity e);

	static void detachTransform(entt::registry& reg, entt::entity e);

	static void updateTransform(entt::registry& reg, entt::entity e);
};