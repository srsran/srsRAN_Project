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

#pragma once

#include "srsran/support/srsran_assert.h"
#include <atomic>
#include <limits>
#include <type_traits>

namespace srsran {

namespace detail {

/// \brief Lock-free stack of indexes.
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
    // We use memory ordering "acquire" to form a "synchronizes-with" relationship with the release in push().
    // The "acquire" ordering also ensures that the next_idx[old_top.index] read is not reordered to happen before the
    // atomic operation.
    // In case of failure, "top" remains unchanged, so the operation can have "relaxed" ordering.
    while (not top.compare_exchange_weak(old_top, new_top, std::memory_order_acquire, std::memory_order_relaxed)) {
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
    // We use memory ordering "release" for success path to form a "synchronizes-with" relationship with the acquire in
    // pop(). The "release" ordering also ensures that the next_idx[index] write is visible to other threads.
    // In case of failure, "top" remains unchanged, so the operation can have "relaxed" ordering.
    while (not top.compare_exchange_weak(old_top, new_top, std::memory_order_release, std::memory_order_relaxed)) {
      new_top.epoch   = old_top.epoch + 1;
      next_idx[index] = old_top.index;
    }
  }

private:
  struct node {
    index_type index;
    index_type epoch;
  };
  static_assert(sizeof(node) == sizeof(uint64_t),
                "node sizeof should no go beyond 64-bits, otherwise we lose is_lock_free guarantees in most platforms");

  std::atomic<node>       top;
  std::vector<index_type> next_idx;
};

} // namespace detail

/// \brief Lock-free stack of objects with bounded capacity.
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

/// \brief Lock-free pool of objects.
template <typename T>
class lockfree_object_pool
{
  using index_type = detail::lockfree_index_stack::index_type;

  static constexpr index_type npos() { return detail::lockfree_index_stack::npos(); }

  struct pool_deleter {
    lockfree_object_pool* parent;
    void                  operator()(T* t)
    {
      if (t != nullptr) {
        parent->release_raw(t);
      }
    }
  };

public:
  /// Pointer type to object managed by the object pool.
  using ptr = std::unique_ptr<T, pool_deleter>;

  lockfree_object_pool(unsigned nof_elems, const T& val = {}) :
    objects(nof_elems, val), free_list(nof_elems, true), sz_estim(nof_elems)
  {
    srsran_assert(nof_elems > 0, "Invalid pool size={}", nof_elems);
  }

  /// Allocate new pool object with managed lifetime.
  ptr get() { return ptr{get_raw(), pool_deleter{this}}; }

  /// Maximum number of objects managed by this pool.
  size_t capacity() const { return objects.size(); }

  /// Estimation of the available objects currently stored in this pool.
  size_t estimated_size() const { return sz_estim.load(std::memory_order_relaxed); }

  T* get_raw()
  {
    index_type popped_idx = free_list.try_pop();
    if (popped_idx == npos()) {
      // Capacity exceeded.
      return nullptr;
    }
    sz_estim.fetch_sub(1, std::memory_order_relaxed);
    return &objects[popped_idx];
  }

  void release_raw(T* node)
  {
    srsran_assert(node >= objects.data() and node < objects.data() + objects.size(),
                  "Deallocated nodes must be within the pool");
    index_type idx = node - objects.data();
    sz_estim.fetch_add(1, std::memory_order_relaxed);
    free_list.push(idx);
  }

private:
  // List of created objects.
  std::vector<T> objects;

  // List of free indexes of the objects vector.
  detail::lockfree_index_stack free_list;

  std::atomic<unsigned> sz_estim;
};

} // namespace srsran