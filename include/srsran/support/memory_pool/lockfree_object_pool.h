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
#include "srsran/adt/unique_function.h"
#include <atomic>
#include <type_traits>

namespace srsran {

namespace detail {

class lockfree_index_stack
{
public:
  using index_type = uint32_t;

  static constexpr index_type npos() { return std::numeric_limits<index_type>::max(); }

  lockfree_index_stack(uint32_t nof_indexes, bool start_full) :
    top(node{start_full ? 0 : npos(), 0}), next_idx(nof_indexes, npos())
  {
    srsran_assert(nof_indexes > 0 and nof_indexes < npos(), "Invalid stack size={}", nof_indexes);

    // Initialize the stack of next indexes like [1, 2, 3, ..., nof_indexes - 1, npos]
    for (index_type i = 1; i < nof_indexes; ++i) {
      next_idx[i - 1] = i;
    }
  }

  index_type capacity() const { return next_idx.size(); }

  index_type try_pop()
  {
    node old_top{top.load(std::memory_order_relaxed)};
    if (old_top.index == npos()) {
      return npos();
    }
    node new_top{next_idx[old_top.index], old_top.epoch + 1};
    while (not top.compare_exchange_weak(old_top, new_top)) {
      if (old_top.index == npos()) {
        return npos();
      }
      new_top = node{next_idx[old_top.index], old_top.epoch + 1};
    }
    return old_top.index;
  }

  void push(index_type index)
  {
    node old_top{top.load(std::memory_order_relaxed)};
    next_idx[index] = old_top.index;
    node new_top{index, old_top.epoch + 1};
    while (not top.compare_exchange_weak(old_top, new_top)) {
      new_top.epoch   = old_top.epoch + 1;
      next_idx[index] = old_top.index;
    }
  }

private:
  struct node {
    index_type index;
    index_type epoch;
  };

  std::atomic<node>       top;
  std::vector<index_type> next_idx;
};

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
  lockfree_offset_stack(node_t* pool_start_) : pool_start(reinterpret_cast<uint8_t*>(pool_start_)) {}

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
class lockfree_bounded_stack
{
  using index_type = detail::lockfree_index_stack::index_type;

  static constexpr index_type npos() { return detail::lockfree_index_stack::npos(); }

public:
  lockfree_bounded_stack(unsigned capacity) : objects(capacity), free_list(capacity, true), stack(capacity, false) {}

  bool push(const T& item)
  {
    index_type popped_idx = free_list.try_pop();
    if (popped_idx == npos()) {
      // Capacity exceeded.
      return false;
    }
    objects[popped_idx] = item;
    stack.push(popped_idx);
    sz_estim.fetch_add(1, std::memory_order_relaxed);
    return true;
  }

  bool pop(T& popped_item)
  {
    index_type popped_idx = stack.try_pop();
    if (popped_idx == npos()) {
      return false;
    }
    popped_item = std::move(objects[popped_idx]);
    free_list.push(popped_idx);
    sz_estim.fetch_sub(1, std::memory_order_relaxed);
    return true;
  }

  size_t size() const { return sz_estim.load(std::memory_order_relaxed); }

  size_t capacity() const { return objects.size(); }

private:
  std::vector<T> objects;

  detail::lockfree_index_stack free_list;
  detail::lockfree_index_stack stack;

  std::atomic<index_type> sz_estim{0};
};

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

  lockfree_object_pool(size_t nof_elems, const T& val = {}) :
    objects(nof_elems, val),
    offset_obj_to_node((size_t)(&(objects[0].obj)) - (size_t)&objects[0]),
    free_list(objects.data()),
    estim_size(nof_elems)
  {
    srsran_assert(nof_elems > 0, "Invalid pool size={}", nof_elems);

    for (unsigned i = 0; i != nof_elems; ++i) {
      free_list.push(&objects[i]);
    }
  }

  lockfree_object_pool(size_t nof_elems, unique_function<T()> factory) :
    objects([nof_elems, factory = std::move(factory)]() mutable {
      srsran_assert(nof_elems > 0, "Invalid pool size={}", nof_elems);
      std::vector<node> vec;
      vec.reserve(nof_elems);
      for (unsigned i = 0; i != nof_elems; ++i) {
        vec.emplace_back(factory());
      }
      return vec;
    }()),
    offset_obj_to_node((size_t)(&(objects[0].obj)) - (size_t)&objects[0]),
    free_list(objects.data()),
    estim_size(nof_elems)
  {
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
  void deallocate(T* o) noexcept
  {
    node* node_ptr = reinterpret_cast<node*>(reinterpret_cast<uint8_t*>(o) - offset_obj_to_node);
    free_list.push(node_ptr);
    estim_size.fetch_add(1, std::memory_order_relaxed);
  }

  std::vector<node> objects;

  size_t offset_obj_to_node;

  detail::lockfree_offset_stack free_list;

  std::atomic<unsigned> estim_size;
};

} // namespace srsran