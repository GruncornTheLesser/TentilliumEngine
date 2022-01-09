#pragma once
#include<stdint.h>
#include "ModelComponent.h"

class AnimatorComponent {};
class LightComponent {};
class ShadowComponent {};
class PostProcessComponent {};





	// entity w model must have Hierarchy
	// if no transform component its the root of a scene
#include "HierarchyComponent.h"
class TransformComponent {};