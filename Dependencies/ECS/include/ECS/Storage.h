#pragma once

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <limits>
#include <memory>
#include <tuple>
#include <utility>


template<typename Entity>
class sparse_array final
{
public:
    using entity_type = Entity;
    using size_type = size_t;
    using array_type = entity_type*;
    using shared_count_type = uint16_t;

    static_assert(std::numeric_limits<entity_type>::is_integer && !std::numeric_limits<entity_type>::is_signed,
        "Entity type must be an unsigned integer");

    sparse_array()
        : _array(NULL), _capacity(0), _shared(0)
    {}

    ~sparse_array()
    {
        if (_array) free(_array);
    }

    sparse_array(const sparse_array&) = delete;
    sparse_array(sparse_array&&) = delete;
    sparse_array& operator=(const sparse_array&) = delete;
    sparse_array& operator=(sparse_array&&) = delete;

    void assure(const entity_type entity)
    {
        if (entity >= _capacity)
        {
            const auto linear = entity + (1024 / sizeof(entity_type)); // 1kb
            const auto exponential = _capacity << 1; // Double capacity

            _capacity = entity >= exponential ? linear : exponential;

            _array = static_cast<array_type>(std::realloc(_array, _capacity * sizeof(entity_type)));
        }
    }

    entity_type operator[](const entity_type entity) const { return _array[entity]; }
    entity_type& operator[](const entity_type entity) { return _array[entity]; }

    size_type capacity() const { return _capacity; }
    void share() { ++_shared; }
    void unshare() { --_shared; }

    shared_count_type shared() const { return _shared; }

private:
    array_type _array;
    size_type _capacity;
    shared_count_type _shared;
};

template<typename Entity, typename Archetype>
class storage;

template<typename Entity, typename... Components>
class storage<Entity, Archetype<Components...>> final
{
public:
    using entity_type = Entity;
    using size_type = size_t;

    template<typename Component>
    static constexpr bool contains_component = contains_v<Component, Set<Components...>>;

private:
    using dense_type = entity_type*;
    using page_type = entity_type*;
    using sparse_type = sparse_array<Entity>*;
    using component_pool_type = std::tuple<Components*...>;

    static_assert(std::numeric_limits<entity_type>::is_integer && !std::numeric_limits<entity_type>::is_signed,
        "Entity type must be an unsigned integer");

public:
    class iterator;

    storage()
        : _dense(NULL), _size(0), _capacity(0)
    {
        // Uses new, but normally when using shared sparse arrays it will be allocated on the stack
        _sparse = new sparse_array<entity_type>();

        // Allocate nothing by default
        ((access<Components>() = NULL), ...);
    }

    ~storage()
    {
        // We only manage our sparse_array memory if its not shared
        if (_sparse->shared()) _sparse->unshare();
        else
            delete _sparse;

        // We assume that if dense is NULL, the other arrays are NULL since
        // they grow together.
        if (_dense)
        {
            free(_dense);
            (deallocate<Components>(), ...);
        }
    }

    storage(const storage&) = delete;
    storage(storage&&) = delete;
    storage& operator=(const storage&) = delete;
    storage& operator=(storage&&) = delete;

    template<typename... IncludedComponents>
    void insert(const entity_type entity, const IncludedComponents&... components)
    {
        static_assert(contains_all_v<Set<Components...>, IncludedComponents...>,
            "One or more included components do not belong to the Archetype");
        static_assert(unique_types_v<IncludedComponents...>,
            "Included components are not unique");

        if (_size == _capacity) grow();
        _sparse->assure(entity);

        _dense[_size] = entity;

        // Call the constructors if needed
        (construct<Components>(_size), ...);

        ((access<IncludedComponents>()[_size] = components), ...);

        (*_sparse)[entity] = static_cast<entity_type>(_size++);
    }

    void erase(const entity_type entity)
    {
        const auto back_entity = _dense[--_size];
        const auto index = (*_sparse)[entity];

        (*_sparse)[back_entity] = index;
        _dense[index] = back_entity;

        // Call the destructors if needed
        (destroy<Components>(index), ...);

        // Moves the component data to the new location
        ((access<Components>()[index] = std::move(access<Components>()[_size])), ...);
    }

    bool contains(const entity_type entity) const
    {
        size_type index;

        // We must access the dense array here because our sparse arrays may be shared, therefor we need
        // to make sure entity index is valid.
        return entity < _sparse->capacity() && (index = (*_sparse)[entity]) < _size&& _dense[index] == entity;
    }

    template<typename Component>
    Component& unpack(const entity_type entity)
    {
        static_assert(contains_v<Component, Set<Components...>>,
            "The component your trying to unpack does not belong to the Archetype");

        return access<Component>()[(*_sparse)[entity]];
    }

    void shrink_to_fit()
    {
        if (_size != _capacity)
        {
            _capacity = _size;

            _dense = static_cast<dense_type>(std::realloc(_dense, _capacity * sizeof(entity_type)));
            (reallocate<Components>(), ...);
        }
    }

    void share(sparse_type sparse)
    {
        if (_size != 0) return;

        if (_sparse->shared()) _sparse->unshare();
        else
            // If the sparse_array is not shared that means we current have the default sparse_array
            // allocated at initialization and we must delete it.
            delete _sparse;

        _sparse = sparse;
        sparse->share();
    }

    void clear() { _size = 0; }

    iterator begin() { return { this, _size - 1 }; }
    iterator end() { return { this, static_cast<size_type>(-1) }; }

    [[nodiscard]] size_type size() const { return _size; }
    [[nodiscard]] size_type capacity() const { return _capacity; }
    [[nodiscard]] bool empty() const { return _size == 0; }

private:

    void grow()
    {
        // This is essentially _capacity * 1.5 + 8
        // Note: Must try to find optimal growth rate for better reallocation
        _capacity = (_capacity * 3) / 2 + 8;

        // Grow all arrays together
        _dense = static_cast<dense_type>(std::realloc(_dense, _capacity * sizeof(entity_type)));
        (reallocate<Components>(), ...);
    }

    template<typename Component>
    void deallocate()
    {
        if constexpr (!std::is_trivially_destructible_v<Component>)
        {
            for (size_t i = 0; i < _size; i++)
            {
                access<Component>()[i].~Component();
            }
        }

        free(access<Component>());
    }

    template<typename Component>
    void reallocate()
    {
        if (std::is_trivially_copyable_v<Component> || std::is_trivially_move_assignable_v<Component>)
        {
            access<Component>() = static_cast<Component*>(std::realloc(access<Component>(), _capacity * sizeof(Component)));
        }
        else
        {
            Component* old_array = access<Component>();

            Component* new_array = static_cast<Component*>(std::malloc(_capacity * sizeof(Component)));

            for (size_t i = 0; i < _size; i++)
            {
                if constexpr (!std::is_trivially_constructible_v<Component>)
                {
                    new (new_array + i) Component();
                }

                new_array[i] = std::move(old_array[i]);

                old_array[i].~Component();
            }

            free(old_array);

            access<Component>() = new_array;
        }
    }

    template<typename Component>
    void construct(const size_type index)
    {
        if constexpr (!std::is_trivially_constructible_v<Component>)
        {
            new (access<Component>() + index) Component(); // Default constructor
        }
        else
            (void)index; // Suppress unused warning
    }

    template<typename Component>
    void destroy(const size_type index)
    {
        if constexpr (!std::is_trivially_destructible_v<Component>)
        {
            access<Component>()[index].~Component();
        }
        else
            (void)index; // Suppress unused warning
    }

    template<typename Component>
    Component*& access()
    {
        static_assert(contains_v<Component, Set<Components...>>,
            "The component type your trying to access does not belong to the Archetype");

        return std::get<Component*>(_pool);
    }

private:
    dense_type _dense;
    sparse_type _sparse;
    component_pool_type _pool;

    size_type _size;
    size_type _capacity;
};

template<typename Entity, typename... Components>
class storage<Entity, Archetype<Components...>>::iterator final
{
public:
    using iterator_category = std::random_access_iterator_tag;

    iterator(storage* const ptr, const size_type pos)
        : _ptr{ ptr }, _pos{ pos }
    {}

    iterator& operator+=(const size_type value) { _pos -= value; return *this; }
    iterator& operator-=(const size_type value) { _pos += value; return *this; }

    iterator& operator++() { return --_pos, * this; }
    iterator& operator--() { return ++_pos, * this; }

    iterator operator+(const size_type value) const { return { _ptr, _pos - value }; }
    iterator operator-(const size_type value) const { return { _ptr, _pos + value }; }

    bool operator==(const iterator other) const { return other._pos == _pos; }
    bool operator!=(const iterator other) const { return other._pos != _pos; }

    bool operator<(const iterator other) const { return other._pos > _pos; }
    bool operator>(const iterator other) const { return other._pos < _pos; }

    bool operator<=(const iterator other) const { return other._pos >= _pos; }
    bool operator>=(const iterator other) const { return other._pos <= _pos; }

    [[nodiscard]] entity_type operator*() const { return _ptr->_dense[_pos]; }

    template<typename Component>
    [[nodiscard]] const Component& unpack() const
    {
        return _ptr->access<Component>()[_pos];
    }

    template<typename Component>
    [[nodiscard]] Component& unpack()
    {
        return const_cast<Component&>(const_cast<const iterator*>(this)->unpack<Component>());
    }

private:
    storage* const _ptr;
    size_type _pos;
};