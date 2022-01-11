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

    // instantiate
    for (int i = 0; i < 10; i++)
    {
        auto entity = registry.create();
        position& ref1 = registry.emplace<position>(entity, (float)i, (float)i);
        velocity& ref2 = registry.emplace<velocity>(entity, (float)i, (float)i);
        std::cout << "there are " << registry.alive() << " entities alive." << std::endl;
    }
    // sort
    registry.sort<position>([](const auto& lhs, const auto& rhs) { return lhs.x > rhs.x; });

    // iterate
    auto view = registry.view<position, velocity>();
    view.each([](auto& pos, auto& vel) { std::cout << pos.x << std::endl; });
    view.each([](const auto entity, const auto& pos, auto& vel) { /* ... */ });
    for (auto [entity, pos, vel] : view.each()) { /* ... */ }
        

}


