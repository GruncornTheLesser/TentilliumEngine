#pragma once
#include "../Components/Hierarchy.h"
#include "entt_macros.h"
class HierarchySystem : virtual protected entt::registry {

private:
	VIEW(viewHierarchy, GET(Hierarchy), EXC());

public:
	void update();

};

