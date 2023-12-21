/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file Implementation of lock-free intrusive stack.
///
/// Implementation is inspired on
/// https://www.codeproject.com/Articles/801537/A-Fundamental-Lock-Free-Building-Block-The-Lock-Fr.

#pragma once

#include "memory_block_list.h"
#include <atomic>
#include <type_traits>

namespace srsran {

namespace detail {

class lockfree_stack_node
{
public:
  using node_offset = uint32_t;
  using epoch_index = uint32_t;

  static constexpr node_offset invalid_offset = std::numeric_limits<node_offset>::max();

  node_offset next_offset;
  epoch_index epoch;
};

class lockfree_offset_stack
{
  using node_t = lockfree_stack_node;

public:
  lockfree_offset_stack(uint8_t* pool_start_) : pool_start(pool_start_) {}

  /// Pushes a new memory block to the stack.
  void push(node_t* n) noexcept
  {
    node_t old_head{node_t::invalid_offset, 0};
    node_t new_head{get_offset(*n), 0};
    n->next_offset = node_t::invalid_offset;
    while (not head.compare_exchange_weak(old_head, new_head)) {
      n->next_offset = old_head.next_offset;
      new_head.epoch = old_head.epoch + 1;
    }
  }

  SRSRAN_NODISCARD bool pop(node_t*& n)
  {
    node_t old_head{node_t::invalid_offset, 0};
    node_t new_head{node_t::invalid_offset, 0};
    n = nullptr;
    while (not head.compare_exchange_weak(old_head, new_head)) {
      n = get_next_ptr(old_head);
      if (n == nullptr) {
        break;
      }
      new_head = node_t{n->next_offset, old_head.epoch + 1};
    }
    return n != nullptr;
  }

private:
  node_t* get_next_ptr(const node_t& n)
  {
    return n.next_offset != node_t::invalid_offset ? reinterpret_cast<node_t*>(pool_start + n.next_offset) : nullptr;
  }
  node_t::node_offset get_offset(const node_t& n) { return reinterpret_cast<const uint8_t*>(&n) - pool_start; }

  std::atomic<node_t> head{node_t{node_t::invalid_offset, 0}};

  uint8_t* pool_start = nullptr;
};

} // namespace detail

template <typename T>
class lockfree_object_pool
{
  struct node : public detail::lockfree_stack_node {
    T obj;

    node(const T& obj_) : obj(obj_) {}
  };

  struct custom_deleter {
    lockfree_object_pool<T>* pool;

    void operator()(T* t)
    {
      if (t != nullptr) {
        pool->deallocate(t);
      }
    }
  };

public:
  using ptr = std::unique_ptr<T, custom_deleter>;

  lockfree_object_pool(size_t nof_elems, const T& val) :
    objects(nof_elems, val),
    offset_obj_to_node((size_t)(&(objects[0].obj)) - (size_t)&objects[0]),
    free_list(reinterpret_cast<uint8_t*>(objects.data())),
    estim_size(nof_elems)
  {
    srsran_assert(nof_elems > 0, "Invalid pool size={}", nof_elems);

    for (unsigned i = 0; i != nof_elems; ++i) {
      free_list.push(&objects[i]);
    }
  }

  ptr allocate() noexcept
  {
    detail::lockfree_stack_node* popped_node;
    if (free_list.pop(popped_node)) {
      estim_size.fetch_sub(1, std::memory_order_relaxed);
      return ptr{&static_cast<node*>(popped_node)->obj, custom_deleter{this}};
    }
    return ptr{nullptr, custom_deleter{this}};
  }

  size_t capacity() const { return objects.size(); }

  size_t estimated_size() const { return estim_size.load(std::memory_order_relaxed); }

private:
  void deallocate(T* o)
  {
    estim_size.fetch_add(1, std::memory_order_relaxed);
    node* node_ptr = reinterpret_cast<node*>(reinterpret_cast<uint8_t*>(o) - offset_obj_to_node);
    free_list.push(node_ptr);
  }

  std::vector<node> objects;

  const size_t offset_obj_to_node;

  detail::lockfree_offset_stack free_list;

  std::atomic<unsigned> estim_size;
};

} // namespace srsran