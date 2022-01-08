#pragma once
#include "ECS/Archetype.h"
#include "ECS/EntityManager.h"
#include "ECS/Storage.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

using entityID = uint32_t;

template<typename ArchetypeSet>
class Registry;

template<typename... Archetypes>
class Registry<Set<Archetypes...>> //: verify_Archetype<Set<Archetypes...>>
{
public:
    static_assert(sizeof...(Archetypes) > 0, "Registry must contain atleast one Archetype");

private:
    template<typename... Components>
    class basic_view;
private:
    std::tuple<storage<entityID, Archetypes>...>    _pool;
    sparse_array<entityID>                          _shared;
    EntityManager<entityID>                         _manager;
public:
    Registry();

    Registry(const Registry&) = delete;
    Registry(Registry&&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry& operator=(Registry&&) = delete;

    template<typename... Components>
    entityID create();

    template<typename... Components>
    entityID Create(const Components&... components);

    template<typename... Components>
    void Destroy(const entityID entity);
    
    void Destroy_all();
    
    void Optimize();

    template<typename... Components, typename Callable>
    void for_each(const Callable& callable);

    template<typename... Components>
    void swap_Archetype(const entityID entity);

    template<typename Component>
    Component& unpack(const entityID entity);

    template<typename... Components>
    bool has(const entityID entity);

    template<typename... Components>
    size_t size();

    template<typename... Components>
    bool empty();

    template<typename... Components>
    auto view();

    size_t storages() const;

    template<typename Archetype>
    auto& access();

private:
    template<size_t I = 0>
    void setup_shared_memory();
};




template<typename... Archetypes>
template<typename... Components>
class Registry<Set<Archetypes...>>::basic_view
{
public:
    using Archetype_Set_view_type = prune_for_t<Set<Archetypes...>, Components...>;
    static_assert(size_v<Archetype_Set_view_type> > 0, "There are no archetypes matching components.");

private:
    Registry<Set<Archetypes...>>* _registry;

public:
    explicit basic_view(Registry<Set<Archetypes...>>* registry) : _registry{ registry } { }

    template<typename... SwapComponents>
    void swap_Archetype(const entityID entity);

    void Destroy(const entityID entity);

    template<typename Callable>
    void for_each(const Callable& callable);

    template<typename Component>
    Component& unpack(const entityID entity);

    bool contains(const entityID entity);

    size_t size();

    bool empty();

private:
    template<size_t I, typename Callable>
    void r_for_each(const Callable& callable);

    template<size_t I, typename Callable>
    void r_apply(const entityID entity, const Callable& callable);

    template<typename Component, size_t I>
    Component& r_unpack(const entityID entity);

    template<size_t I>
    bool r_contains(const entityID entity);

    template<size_t I>
    size_t r_size();

    template<size_t I>
    bool r_empty();

    template<typename Component, typename Storage, typename Tuple>
    void try_transfer(const entityID entity, Storage& storage, Tuple& temp);
};



template<class ... Archetypes>
Registry<Set<Archetypes...>>::Registry()
{
    setup_shared_memory();
}

template<class ... Archetypes>
template<class ... Components>
entityID Registry<Set<Archetypes...>>::Create(const Components&... components)
{
    using archetype = find_for_t<Set<Archetypes...>, Components...>;

    static_assert(size_v<archetype> == sizeof...(Components),
        "Registry does not contain suitable Archetype for provided components");

    const entityID entity = _manager.generate();

    access<archetype>().insert(entity, components...);

    return entity;
}

template<class ... Archetypes>
template<class ... Components>
entityID Registry<Set<Archetypes...>>::create()
{
    using archetype = find_for_t<Set<Archetypes...>, Components...>;

    static_assert(size_v<archetype> == sizeof...(Components),
        "Registry does not contain suitable Archetype for provided components");

    const entityID entity = _manager.generate();

    access<archetype>().insert(entity, Components{}...);

    return entity;
}

template<class ... Archetypes>
template<typename... Components>
void Registry<Set<Archetypes...>>::Destroy(const entityID entity)
{
    static_assert(size_v<prune_for_t<Set<Archetypes...>, Components...>> > 0,
        "Registry does not contain suitable Archetype for provided components");

    view<Components...>().Destroy(entity);
}

template<class ... Archetypes>
void Registry<Set<Archetypes...>>::Destroy_all()
{
    ((access<Archetypes>().clear()), ...);

    _manager.release_all();
}

template<class ... Archetypes>
void Registry<Set<Archetypes...>>::Optimize()
{
    ((access<Archetypes>().shrink_to_fit()), ...);

    _manager.swap();
    _manager.shrink_to_fit();
}

template<class ... Archetypes>
template<typename... Components, typename Callable>
void Registry<Set<Archetypes...>>::for_each(const Callable& callable) 
{ 
    view<Components...>().for_each(callable); 
}

template<class ... Archetypes>
template<typename... Components>
void Registry<Set<Archetypes...>>::swap_Archetype(const entityID entity) 
{ 
    view().template swap_Archetype<Components...>(entity); 
}

template<class ... Archetypes>
template<typename Component>
Component& Registry<Set<Archetypes...>>::unpack(const entityID entity) 
{ 
    return view<Component>().template unpack<Component>(entity); 
}

template<class ... Archetypes>
template<typename... Components>
bool Registry<Set<Archetypes...>>::has(const entityID entity) 
{ 
    return view<Components...>().contains(entity); 
}

template<class ... Archetypes>
template<typename... Components>
size_t Registry<Set<Archetypes...>>::size() 
{ 
    return view<Components...>().size(); 
}

template<class ... Archetypes>
template<typename... Components>
bool Registry<Set<Archetypes...>>::empty() 
{ 
    return view<Components...>().empty(); 
}

template<class ... Archetypes>
template<typename... Components>
auto Registry<Set<Archetypes...>>::view() 
{ 
    return basic_view<Components...> { this };
}

template<class ... Archetypes>
size_t Registry<Set<Archetypes...>>::storages() const 
{ 
    return _shared.shared(); 
}

template<class ... Archetypes>
template<typename Archetype>
auto& Registry<Set<Archetypes...>>::access() 
{ 
    return std::get<storage<entityID, Archetype>>(_pool); 
}

template<class ... Archetypes>
template<size_t I>
void Registry<Set<Archetypes...>>::setup_shared_memory()
{
    using current = at_t<I, Set<Archetypes...>>;

    if constexpr (I < size_v<Set<Archetypes...>>)
    {
        access<current>().share(&_shared);
        setup_shared_memory<I + 1>();
    }
}





template<typename... Archetypes>
template<typename... Components>
template<typename... SwapComponents>
void Registry<Set<Archetypes...>>::basic_view<Components...>::swap_Archetype(const entityID entity)
{
    static_assert(size_v<prune_for_t<Set<Archetypes...>, SwapComponents...>> > 0,
        "The Archetype to swap to does not exist.");

    using new_Archetype = Archetype<SwapComponents...>;

    r_apply<0>(entity, [this](auto& s, const entityID e)
        {
            std::tuple<SwapComponents...> temp;
            ((try_transfer<SwapComponents>(e, s, temp)), ...);

            s.erase(e);

            _registry->template access<new_Archetype>()
                .insert(e, std::move(std::get<SwapComponents>(temp))...);
        });
}

template<typename... Archetypes>
template<typename... Components>
void Registry<Set<Archetypes...>>::basic_view<Components...>::Destroy(const entityID entity)
{
    r_apply<0>(entity, [](auto& s, const entityID e) { s.erase(e); });
    _registry->_manager.release(entity);
}

template<typename... Archetypes>
template<typename... Components>
template<typename Callable>
void Registry<Set<Archetypes...>>::basic_view<Components...>::for_each(const Callable& callable)
{
    r_for_each<0, Callable>(callable);
}

template<typename... Archetypes>
template<typename... Components>
template<typename Component>
Component& Registry<Set<Archetypes...>>::basic_view<Components...>::unpack(const entityID entity)
{
    static_assert(size_v<prune_for_t<Archetype_Set_view_type, Component>> > 0, "You cannot unpack a component type that is not included in the Registry");

    return r_unpack<Component, 0>(entity);
}

template<typename... Archetypes>
template<typename... Components>
bool Registry<Set<Archetypes...>>::basic_view<Components...>::contains(const entityID entity)
{
    return r_contains<0>(entity);
}

template<typename... Archetypes>
template<typename... Components>
size_t Registry<Set<Archetypes...>>::basic_view<Components...>::size()
{
    return r_size<0>();
}

template<typename... Archetypes>
template<typename... Components>
bool Registry<Set<Archetypes...>>::basic_view<Components...>::empty()
{
    return r_empty<0>();
}

template<typename... Archetypes>
template<typename... Components>
template<size_t I, typename Callable>
void Registry<Set<Archetypes...>>::basic_view<Components...>::r_for_each(const Callable& callable)
{
    auto& storage = _registry->template access<at_t<I, Archetype_Set_view_type>>();

    for (auto it = storage.begin(); it != storage.end(); ++it)
    {
        callable(*it, it.template unpack<Components>()...);
    }

    if constexpr (I + 1 < size_v<Archetype_Set_view_type>) r_for_each<I + 1>(callable);
}

template<typename... Archetypes>
template<typename... Components>
template<size_t I, typename Callable>
void Registry<Set<Archetypes...>>::basic_view<Components...>::r_apply(const entityID entity, const Callable& callable)
{
    auto& storage = _registry->template access<at_t<I, Archetype_Set_view_type>>();

    // If we assume that the entity is in atleast one of the storages in the view,
    // we can skip the verification for the last possible storage.
    if constexpr (I == size_v<Archetype_Set_view_type> -1) callable(storage, entity);
    else if (storage.contains(entity))
        callable(storage, entity);
    else
        r_apply<I + 1, Callable>(entity, callable);
}

template<typename... Archetypes>
template<typename... Components>
template<typename Component, size_t I>
Component& Registry<Set<Archetypes...>>::basic_view<Components...>::r_unpack(const entityID entity)
{
    auto& storage = _registry->template access<at_t<I, Archetype_Set_view_type>>();

    // If we assume that the entity is in atleast one of the storages in the view,
    // we can skip the verification for the last possible storage.
    if constexpr (I == size_v<Archetype_Set_view_type> -1)
        return storage.template unpack<Component>(entity);
    else if (storage.contains(entity))
        return storage.template unpack<Component>(entity);
    else
        return r_unpack<Component, I + 1>(entity);
}

template<typename... Archetypes>
template<typename... Components>
template<size_t I>
bool Registry<Set<Archetypes...>>::basic_view<Components...>::r_contains(const entityID entity)
{
    if (_registry->template access<at_t<I, Archetype_Set_view_type>>().contains(entity)) return true;

    if constexpr (I + 1 == size_v<Archetype_Set_view_type>) return false;
    else
        return r_contains<I + 1>(entity);
}

template<typename... Archetypes>
template<typename... Components>
template<size_t I>
size_t Registry<Set<Archetypes...>>::basic_view<Components...>::r_size()
{
    if constexpr (I == size_v<Archetype_Set_view_type>)
        return 0;
    else
        return _registry->template access<at_t<I, Archetype_Set_view_type>>().size() + r_size<I + 1>();
}

template<typename... Archetypes>
template<typename... Components>
template<size_t I>
bool Registry<Set<Archetypes...>>::basic_view<Components...>::r_empty()
{
    if constexpr (I == size_v<Archetype_Set_view_type>)
        return true;
    else if (!_registry->template access<at_t<I, Archetype_Set_view_type>>().empty())
        return false;
    else
        return r_empty<I + 1>();
}

template<typename... Archetypes>
template<typename... Components>
template<typename Component, typename Storage, typename Tuple>
void Registry<Set<Archetypes...>>::basic_view<Components...>::try_transfer(const entityID entity, Storage& storage, Tuple& temp)
{
    if constexpr (storage.contains_component<Component>)
        std::get<Component>(temp) = std::move(storage.template unpack<Component>(entity));
}
