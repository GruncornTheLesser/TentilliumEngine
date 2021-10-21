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


template<typename Entity, typename ArchetypeSet>
class Registry;

template<typename Entity, typename... Archetypes>
class Registry<Entity, Set<Archetypes...>> : verify_Archetype<Set<Archetypes...>>
{
public:
    using entity_type = Entity;
    using Archetype_Set_type = Set<Archetypes...>;
    using registry_type = Registry<entity_type, Archetype_Set_type>;
    using pool_type = std::tuple<storage<entity_type, Archetypes>...>;
    using shared_type = sparse_array<entity_type>;
    using manager_type = EntityManager<entity_type>;

    static_assert(std::numeric_limits<entity_type>::is_integer && !std::numeric_limits<entity_type>::is_signed,
        "Entity type must be an unsigned integer");

    static_assert(sizeof...(Archetypes) > 0, "Registry must contain atleast one Archetype");

private:
    template<typename... Components>
    class basic_view;

public:
    Registry()
    {
        setup_shared_memory();
    }
    ~Registry() {}

    Registry(const Registry&) = delete;
    Registry(Registry&&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry& operator=(Registry&&) = delete;

    template<typename... Components>
    entity_type create(const Components&... components)
    {
        using current = find_for_t<Set<Archetypes...>, Components...>;

        static_assert(size_v<current> == sizeof...(Components),
            "Registry does not contain suitable Archetype for provided components");

        const entity_type entity = _manager.generate();

        access<current>().insert(entity, components...);

        return entity;
    }

    template<typename... Components>
    void destroy(const entity_type entity)
    {
        static_assert(size_v<prune_for_t<Set<Archetypes...>, Components...>> > 0,
            "Registry does not contain suitable Archetype for provided components");

        view<Components...>().destroy(entity);
    }

    void destroy_all()
    {
        ((access<Archetypes>().clear()), ...);

        _manager.release_all();
    }

    void optimize()
    {
        ((access<Archetypes>().shrink_to_fit()), ...);

        _manager.swap();
        _manager.shrink_to_fit();
    }

    template<typename... Components, typename Callable>
    void for_each(const Callable& callable) { view<Components...>().for_each(callable); }

    template<typename... Components>
    void swap_Archetype(const entity_type entity) { view().template swap_Archetype<Components...>(entity); }

    template<typename Component>
    Component& unpack(const entity_type entity) { return view<Component>().template unpack<Component>(entity); }

    template<typename... Components>
    bool has(const entity_type entity) { return view<Components...>().contains(entity); }

    template<typename... Components>
    size_t size() { return view<Components...>().size(); }

    template<typename... Components>
    bool empty() { return view<Components...>().empty(); }

    template<typename... Components>
    auto view() { return basic_view<Components...> { this }; }

    size_t storages() const { return _shared.shared(); }

    template<typename Archetype>
    auto& access() { return std::get<storage<entity_type, Archetype>>(_pool); }

private:

    template<size_t I = 0>
    void setup_shared_memory()
    {
        using current = at_t<I, Archetype_Set_type>;

        if constexpr (I < size_v<Archetype_Set_type>)
        {
            access<current>().share(&_shared);
            setup_shared_memory<I + 1>();
        }
    }

private:
    pool_type _pool;
    shared_type _shared;
    manager_type _manager;
};

template<typename Entity, typename... Archetypes>
template<typename... Components>
class Registry<Entity, Set<Archetypes...>>::basic_view
{
public:
    using Archetype_Set_view_type = prune_for_t<Archetype_Set_type, Components...>;

    static_assert(size_v<Archetype_Set_view_type> > 0, "There are no Archetypes in this view");

public:
    explicit basic_view(registry_type* registry) : _registry{ registry } {}

    template<typename... SwapComponents>
    void swap_Archetype(const entity_type entity)
    {
        static_assert(size_v<prune_for_t<Archetype_Set_type, SwapComponents...>> > 0,
            "The Archetype to swap to does not exist.");

        using new_Archetype = Archetype<SwapComponents...>;

        r_apply<0>(entity, [this](auto& s, const entity_type e)
            {
                std::tuple<SwapComponents...> temp;
                ((try_transfer<SwapComponents>(e, s, temp)), ...);

                s.erase(e);

                _registry->template access<new_Archetype>()
                    .insert(e, std::move(std::get<SwapComponents>(temp))...);
            });
    }

    void destroy(const entity_type entity)
    {
        r_apply<0>(entity, [](auto& s, const entity_type e)
            { s.erase(e); });

        _registry->_manager.release(entity);
    }

    template<typename Callable>
    void for_each(const Callable& callable)
    {
        r_for_each<0, Callable>(callable);
    }

    template<typename Component>
    Component& unpack(const entity_type entity)
    {
        static_assert(size_v<prune_for_t<Archetype_Set_view_type, Component>> > 0,
            "You cannot unpack a component type that is not included in the Registry");

        return r_unpack<Component, 0>(entity);
    }

    bool contains(const entity_type entity)
    {
        return r_contains<0>(entity);
    }

    size_t size()
    {
        return r_size<0>();
    }

    bool empty()
    {
        return r_empty<0>();
    }

private:
    template<size_t I, typename Callable>
    void r_for_each(const Callable& callable)
    {
        auto& storage = _registry->template access<at_t<I, Archetype_Set_view_type>>();

        for (auto it = storage.begin(); it != storage.end(); ++it)
        {
            callable(*it, it.template unpack<Components>()...);
        }

        if constexpr (I + 1 < size_v<Archetype_Set_view_type>) r_for_each<I + 1>(callable);
    }

    template<size_t I, typename Callable>
    void r_apply(const entity_type entity, const Callable& callable)
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

    template<typename Component, size_t I>
    Component& r_unpack(const entity_type entity)
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

    template<size_t I>
    bool r_contains(const entity_type entity)
    {
        if (_registry->template access<at_t<I, Archetype_Set_view_type>>().contains(entity)) return true;

        if constexpr (I + 1 == size_v<Archetype_Set_view_type>) return false;
        else
            return r_contains<I + 1>(entity);
    }

    template<size_t I>
    size_t r_size()
    {
        if constexpr (I == size_v<Archetype_Set_view_type>) 
            return 0;
        else
            return _registry->template access<at_t<I, Archetype_Set_view_type>>().size() + r_size<I + 1>();
    }

    template<size_t I>
    bool r_empty()
    {
        if constexpr (I == size_v<Archetype_Set_view_type>) 
            return true;
        else if (!_registry->template access<at_t<I, Archetype_Set_view_type>>().empty())
            return false;
        else
            return r_empty<I + 1>();
    }

    template<typename Component, typename Storage, typename Tuple>
    void try_transfer(const entity_type entity, Storage& storage, Tuple& temp)
    {
        if constexpr (storage.contains_component<Component>)
            std::get<Component>(temp) = std::move(storage.template unpack<Component>(entity));
    }

private:
    registry_type* _registry;
};