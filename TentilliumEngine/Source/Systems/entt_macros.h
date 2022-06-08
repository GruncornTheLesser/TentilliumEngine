#pragma once
#include <entt/entity/registry.hpp>

using entt::literals::operator""_hs;

// important to remember these are just for syntax and wont be enforced in the correct order
// It will always be OWN, GET, EXC
#define OWN(...) __VA_ARGS__
#define GET(...) __VA_ARGS__ 
#define EXC(...) __VA_ARGS__

#define TAG(TAG) using TAG = entt::tag<#TAG##_hs)>
#define VIEW(NAME, GET, EXC) entt::basic_view<entt::entity, entt::get_t<GET>, entt::exclude_t<EXC>, void> NAME = view<GET>(entt::exclude<EXC>)
#define GROUP(NAME, OWN, GET, EXC) entt::basic_group<entt::entity, entt::owned_t<OWN>, entt::get_t<GET>, entt::exclude_t<EXC>> NAME = group<OWN>(entt::get<GET>, entt::exclude<EXC>)
