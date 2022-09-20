#pragma once
#include "../Components/Hierarchy.h"
#include "entt_macros.h"
class HierarchySystem : virtual protected entt::registry {

private:
	VIEW(viewHierarchy, GET(Parent), EXC());

public:
	void update();

};

