#pragma once
#include <cstdlib>
#include <limits>
#include <tuple>
#include <type_traits>

template<typename ... Types>
struct Set
{

};



template<typename Set>
struct size;

template<typename... Types, template<typename...> class Set>
struct size<Set<Types...>> : std::integral_constant<size_t, sizeof...(Types)> { };

template<typename Set> 
constexpr auto size_v = size<Set>::value;



template<typename Set>
struct empty;

template<typename... Types, template<typename...> class Set>
struct empty<Set<Types...>> : std::bool_constant<sizeof...(Types) == 0> { };

template<typename Set>
constexpr auto empty_v = empty<Set>::value;

template<typename Type, typename Set, size_t I = 0>
struct find;

template<typename Type, typename... Types, template<typename...> class Set, size_t I>
struct find<Type, Set<Types...>, I> : std::integral_constant<size_t, I> { };

template<typename Type, typename Head, typename... Types, template<typename...> class Set, size_t I>
struct find<Type, Set<Head, Types...>, I> : std::conditional_t<std::is_same_v<Type, Head>, std::integral_constant<size_t, I>, find<Type, Set<Types...>, I + 1>> { };

template<typename Type, typename Set>
constexpr auto find_v = find<Type, Set, 0>::value;

template<size_t I, typename Set>
struct at;

template<size_t I, typename... Types, template<typename...> class Set>
struct at<I, Set<Types...>> { using type = Set<>; };

template<size_t I, typename Type, typename... Types, template<typename...> class Set>
struct at<I, Set<Type, Types...>>
{
private:
    using next = typename at<I - 1, Set<Types...>>::type;

public:
    using type = typename std::conditional_t<I == 0, Type, next>;
};

template<size_t I, typename Set>
using at_t = typename at<I, Set>::type;

template<typename Type, typename Set>
struct contains;
template<typename Type, typename... Types, template<typename...> class Set>
struct contains<Type, Set<Types...>> : std::disjunction<std::is_same<Type, Types>...> { };
template<typename Type, typename Set>
constexpr auto contains_v = contains<Type, Set>::value;

template<typename Set, typename... Types>
struct contains_all;
template<typename Set, typename... Types>
struct contains_all : std::conjunction<contains<Types, Set>...> { };
template<typename Set, typename... Types>
constexpr auto contains_all_v = contains_all<Set, Types...>::value;

template<typename... Types>
struct unique_types : std::true_type { };
template<typename Type, typename... Types>
struct unique_types<Type, Types...> : std::conjunction<std::negation<std::disjunction<std::is_same<Type, Types>...>>, unique_types<Types...>> { };
template<typename... Types>
constexpr auto unique_types_v = unique_types<Types...>::value;

template<typename ASet, typename BSet>
struct is_same_types;
template<typename... ATypes, template<typename...> class ASet, typename... BTypes, template<typename...> class BSet>
struct is_same_types<ASet<ATypes...>, BSet<BTypes...>> : std::conjunction<std::bool_constant<sizeof...(ATypes) == sizeof...(BTypes)>, contains_all<ASet<ATypes...>, BTypes...>> { };
template<typename ASet, typename BSet>
constexpr auto is_same_types_v = is_same_types<ASet, BSet>::value;

template<typename... Sets>
struct Unique_Sets : std::true_type { };

template<typename Set, typename... Sets>
struct Unique_Sets<Set, Sets...>
    : std::conjunction<std::negation<std::disjunction<is_same_types<Set, Sets>...>>, Unique_Sets<Sets...>>
{};

template<typename... Sets>
constexpr auto Unique_Sets_v = Unique_Sets<Sets...>::value;

template<typename Type, typename Set>
struct push_front;

template<typename Type, typename... Types, template<typename...> class Set>
struct push_front<Type, Set<Types...>>
{
    using type = Set<Type, Types...>;
};

template<typename Type, typename Set>
using push_front_t = typename push_front<Type, Set>::type;

template<typename Type, typename Set>
struct push_back;

template<typename Type, typename... Types, template<typename...> class Set>
struct push_back<Type, Set<Types...>>
{
    using type = Set<Types..., Type>;
};

template<typename Type, typename Set>
using push_back_t = typename push_back<Type, Set>::type;

template<typename SetOfSets, typename... Types>
struct find_for;

template<typename... Sets, template<typename...> class SetOfSets, typename... Types>
struct find_for<SetOfSets<Sets...>, Types...>
{
    using type = Set<>;
};

template<typename HeadSet, typename... Sets, template<typename...> class SetOfSets, typename... Types>
struct find_for<SetOfSets<HeadSet, Sets...>, Types...>
{
private:
    using next = typename find_for<SetOfSets<Sets...>, Types...>::type;

public:
    using type = typename std::conditional_t<is_same_types_v<HeadSet, Set<Types...>>, HeadSet, next>;
};

template<typename SetOfSets, typename... RequiredTypes>
using find_for_t = typename find_for<SetOfSets, RequiredTypes...>::type;

template<typename SetOfSets, typename... RequiredTypes>
struct prune_for;

template<typename... Sets, template<typename...> class SetOfSets, typename... RequiredTypes>
struct prune_for<SetOfSets<Sets...>, RequiredTypes...>
{
    using type = Set<>;
};

template<typename HeadSet, typename... Sets, template<typename...> class SetOfSets, typename... RequiredTypes>
struct prune_for<SetOfSets<HeadSet, Sets...>, RequiredTypes...>
{
private:
    using next = typename prune_for<SetOfSets<Sets...>, RequiredTypes...>::type;
    using to_front = typename push_front<HeadSet, next>::type; // Can make certain operations O(1)
    using to_back = typename push_back<HeadSet, next>::type;
    using accept = typename std::conditional_t<size_v<HeadSet> == sizeof...(RequiredTypes), to_front, to_back>;

public:
    using type = typename std::conditional_t<contains_all<HeadSet, RequiredTypes...>::value, accept, next>;
};

template<typename SetOfSets, typename... RequiredTypes>
using prune_for_t = typename prune_for<SetOfSets, RequiredTypes...>::type;

template<typename Component>
struct verify_component
{
    static_assert(std::is_same_v<Component, std::remove_cv_t<Component>>, "Component cannot be cv-qualified (const or volatile)");
    static_assert(std::is_default_constructible_v<Component>, "Component must be default constructible");
    static_assert(std::is_copy_assignable_v<Component>, "Component must be copy assignable");
    static_assert(std::is_copy_constructible_v<Component>, "Component must be copy constructable");
};

template<typename... Components>
using Archetype = Set<Components...>;

template<typename Archetype>
struct verify_Archetype;

template<typename... Components>
struct verify_Archetype<Archetype<Components...>> : verify_component<Components>...
{
    static_assert(unique_types_v<Components...>, "Every component must be unique (Archetype is a SET of components)");
};

template<typename ArchetypeSet>
struct verify_Archetype_Set;

template<typename... Archetypes>
struct verify_Archetype_Set<Set<Archetypes...>> : verify_Archetype<Archetypes>...
{
    static_assert(Unique_Sets_v<Archetypes...>, "Every Archetype must be unique (regardless of component order)");
};

namespace Internal
{
    template<typename... Archetypes>
    struct Archetype_list_builder
    {
        template<typename Archetype>
        using add = Archetype_list_builder<Archetype, Archetypes...>;
        using build = Set<Archetypes...>;
    };
}

struct Archetype_Set_builder : Internal::Archetype_list_builder<> { };


