#pragma once
#include <vector>
#include "Rendering/AppWindow.h"
#include "Rendering/Shader.h"
#include "Rendering/ResourceManager.h"
#include "Rendering/Image.h"
#include "Components/Components.h"


#include <iostream>
#include <entt/entt.hpp>
using namespace entt;

struct position {
    float x;
    float y;
};

struct velocity {
    float dx;
    float dy;
};

void test()
{
    entt::registry registry;
    
    for (int i = 0; i < 10; i++)
    {
        auto entity = registry.create();
        position& ref1 = registry.emplace<position>(entity, 1.0f, 2.0f);
        std::cout << "there are " << registry.alive() << " entities alive." << std::endl;
    }
    auto view = registry.view<position, velocity>();
    registry.sort<position>([](const auto& lhs, const auto& rhs) { return lhs.x < rhz.x; });
    registry.sort<position>([](const auto& lhs, const auto& rhs) { return lhs.x < rhz.x; });
    view.each([](position& pos, auto& vel) { /* ... */ });
    view.each([](const auto entity, const auto& pos, auto& vel) { /* ... */ });
    for (auto [entity, pos, vel] : view.each()) { /* ... */ }
        

}


