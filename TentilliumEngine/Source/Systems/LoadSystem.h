#pragma once
#include "entt_macros.h"
#include "MaterialSystem.h"

class LoadSystem
	: virtual protected ResourceManager<Texture>, 
	  virtual protected entt::registry 
{
public:
	entt::entity load(std::string filepath);
};