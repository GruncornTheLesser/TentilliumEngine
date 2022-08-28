#pragma once
#include "entt_macros.h"
#include "../Components/Hierarchy.h"
#include "../Components/Transform.h"

class TransformSystem : virtual protected entt::registry {
private:
	
	VIEW(viewRootTransform, GET(Transform), EXC(Hierarchy));
	VIEW(viewTransform, GET(Hierarchy, Transform), EXC());

public:
	TransformSystem() { }

protected:
	void update();
};