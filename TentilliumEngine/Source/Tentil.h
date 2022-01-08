#pragma once
#include <ECS.h>
#include <vector>
#include "Rendering/AppWindow.h"
#include "Rendering/Shader.h"
#include "Rendering/ResourceManager.h"
#include "Rendering/Image.h"
#include "Components/Components.h"

class Scene : public Registry<Component_Set_builder
	//::add<TransformComponent>
	//::add<HierarchyComponent>
	::add<ModelComponent>
	//::add<SkinComponent>
	//::add<LightComponent>
	//::add<ShadowComponent>
	::build>
{
public:
	Scene() {}
	Scene(const char* filepath) 
	{
		// TODO: load gltf file
	}
	
	void Process()
	{

	}

	void Render() 
	{
		// bind appropriate shaders
		for_each<ModelComponent>([](const entityID& entity, ModelComponent& model)
		{
			model.Draw();
		});
	}
};
