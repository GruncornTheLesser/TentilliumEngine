#pragma once
#include "entt_macros.h"
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"

class TransformSystem : virtual protected entt::registry {
private:
	
	VIEW(viewHierarchy, GET(Hierarchy), EXC());
	VIEW(viewTransform, GET(Transform), EXC());
	VIEW(viewRootTransform, GET(Transform), EXC(Hierarchy));
	
public:
	TransformSystem() { }

protected:
	void update();
};