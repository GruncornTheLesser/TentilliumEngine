#pragma once
#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

#define ENTITY_MANAGER_STACK_SIZE 16384 // This should not be changed unless you know what your doing

static_assert((ENTITY_MANAGER_STACK_SIZE& (ENTITY_MANAGER_STACK_SIZE - 1)) == 0,
    "ENTITY_MANAGER_STACK_SIZE must be a power of two");


 template<typename Entity>
 class EntityManager
 {
 public:
     using entity_type = Entity;
     using size_type = size_t;

     static_assert(std::numeric_limits<entity_type>::is_integer && !std::numeric_limits<entity_type>::is_signed,
         "Entity type must be an unsigned integer");

     static constexpr size_type stack_capacity = ENTITY_MANAGER_STACK_SIZE / sizeof(entity_type);

     static constexpr size_type minimum_heap_capacity = stack_capacity * 2;

 public:
     using stack_buffer_type = entity_type[stack_capacity];
     using heap_buffer_type = entity_type*;

     EntityManager()
         : _current(0), _stack_reusable(0), _heap_reusable(0), _heap_capacity(minimum_heap_capacity), _stack_buffer()
     {
         _heap_buffer = static_cast<heap_buffer_type>(std::malloc(minimum_heap_capacity * sizeof(entity_type)));
     }

     ~EntityManager()
     {
         free(_heap_buffer);
     }

     EntityManager(const EntityManager&) = delete;
     EntityManager(EntityManager&&) = delete;
     EntityManager& operator=(const EntityManager&) = delete;
     EntityManager& operator=(EntityManager&&) = delete;

     entity_type generate()
     {
         if (_stack_reusable) return _stack_buffer[--_stack_reusable];
         else if (_heap_reusable)
             return _heap_buffer[--_heap_reusable];
         else
             return _current++;
     }

     void release(entity_type entity)
     {
         if (_stack_reusable < stack_capacity) _stack_buffer[_stack_reusable++] = entity;
         else
         {
             if (_heap_reusable == _heap_capacity)
             {
                 _heap_capacity = (_heap_capacity * 5) / 3;
                 _heap_buffer = static_cast<heap_buffer_type>(std::realloc(_heap_buffer, _heap_capacity * sizeof(entity_type)));
             }
             _heap_buffer[_heap_reusable++] = entity;
         }
     }

     void release_all()
     {
         _stack_reusable = 0;
         _heap_reusable = 0;
         _current = 0;
     }

     void swap()
     {
         if (_heap_reusable && _stack_reusable != stack_capacity)
         {
             const auto stack_space = stack_capacity - _stack_reusable;
             const auto swap_amount = _heap_reusable < stack_space ? _heap_reusable : stack_space;

             void* dst_stack = static_cast<void*>(static_cast<entity_type*>(_stack_buffer) + _stack_reusable);
             void* src_heap = _heap_buffer + _heap_reusable - swap_amount;

             std::memcpy(dst_stack, src_heap, swap_amount * sizeof(entity_type));

             _stack_reusable += swap_amount;
             _heap_reusable -= swap_amount;
         }
     }

     void shrink_to_fit()
     {
         if (_heap_reusable != _heap_capacity && _heap_reusable > minimum_heap_capacity)
         {
             _heap_capacity = _heap_reusable;

             _heap_buffer = static_cast<heap_buffer_type>(std::realloc(_heap_buffer, _heap_capacity * sizeof(entity_type)));
         }
     }

     [[nodiscard]] entity_type peek() const { return _current; }

     [[nodiscard]] size_type stack_reusable() const { return _stack_reusable; }

     [[nodiscard]] size_type heap_reusable() const { return _heap_reusable; }

     [[nodiscard]] size_type reusable() const { return _stack_reusable + _heap_reusable; }

     [[nodiscard]] size_type heap_capacity() const { return _heap_capacity; }

 private:
     entity_type _current;

     size_type _stack_reusable;
     size_type _heap_reusable;
     size_type _heap_capacity;

     heap_buffer_type _heap_buffer;
     stack_buffer_type _stack_buffer;
 };