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
    top(make_node(start_full ? 0 : npos(), 0)), next_idx(new std::atomic<index_type>[nof_indexes]), sz(nof_indexes)
  {
    srsran_assert(nof_indexes > 0 and nof_indexes < npos(), "Invalid stack size={}", nof_indexes);

    // Initialize the stack of next indexes like [1, 2, 3, ..., nof_indexes - 1, npos]
    for (index_type i = 1; i < nof_indexes; ++i) {
      next_idx[i - 1].store(i, std::memory_order_relaxed);
    }
    next_idx[nof_indexes - 1].store(npos(), std::memory_order_relaxed);
  }

  index_type capacity() const { return sz; }

  index_type try_pop()
  {
    // We use memory ordering "acquire" to form a "synchronizes-with" relationship with the release of the last push(),
    // otherwise the write to next_idx[] during the push() is not visible in this thread.
    node old_top{top.load(std::memory_order_acquire)};
    if (get_index(old_top) == npos()) {
      return npos();
    }
    node new_top = make_node(next_idx[get_index(old_top)].load(std::memory_order_relaxed), get_next_aba(old_top));
    // We use memory ordering "acquire" to form a "synchronizes-with" relationship with the release in push().
    // The "acquire" ordering also ensures that the next_idx[old_top.index] read is not reordered to happen before the
    // atomic operation.
    // In case of failure, "top" remains unchanged, but we need to re-read the next_idx[] which could have changed due
    // to a concurrent push(). So, the operation can have "acquired" ordering.
    while (not top.compare_exchange_weak(old_top, new_top, std::memory_order_acquire, std::memory_order_acquire)) {
      if (get_index(old_top) == npos()) {
        return npos();
      }
      new_top = make_node(next_idx[get_index(old_top)].load(std::memory_order_relaxed), get_next_aba(old_top));
    }
    return get_index(old_top);
  }

  void push(index_type index)
  {
    node old_top{top.load(std::memory_order_relaxed)};
    next_idx[index].store(get_index(old_top), std::memory_order_relaxed);
    node new_top = make_node(index, get_aba(old_top));
    // We use memory ordering "release" for success path to form a "synchronizes-with" relationship with the acquire in
    // pop(). The "release" ordering also ensures that the next_idx[index] write is visible to other threads.
    // In case of failure, "top" remains unchanged, so the operation can have "relaxed" ordering.
    while (not top.compare_exchange_weak(old_top, new_top, std::memory_order_release, std::memory_order_relaxed)) {
      set_aba(new_top, get_aba(old_top));
      next_idx[index].store(get_index(old_top), std::memory_order_relaxed);
    }
  }

private:
  using node = uint64_t;

  node     make_node(index_type index, index_type aba) { return (static_cast<uint64_t>(aba) << 32U) | index; }
  uint32_t get_index(node n) const { return n & 0xFFFFFFFF; }
  void     set_index(node& n, index_type index) { n = (n & 0xFFFFFFFF00000000) | index; }
  void     set_aba(node& n, index_type aba) { n = (n & 0xFFFFFFFF) | (static_cast<uint64_t>(aba) << 32U); }
  uint32_t get_aba(node n) const { return static_cast<uint32_t>((n & 0xFFFFFFFF00000000) >> 32U); }
  uint32_t get_next_aba(node n) const { return static_cast<uint32_t>((n & 0xFFFFFFFF00000000) >> 32U) + 1; }

  std::atomic<node> top;

  std::unique_ptr<std::atomic<index_type>[]> next_idx;

  const size_t sz;
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