/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "memory_pool_utils.h"
#include "srsran/support/srsran_assert.h"
#include <mutex>

namespace srsran {

namespace detail {

/// \brief This class represents an intrusive list of memory blocks.
class intrusive_memory_block_list
{
public:
  struct node {
    node* next;
    explicit node(node* prev_) : next(prev_) {}
  };

  node*       head  = nullptr;
  std::size_t count = 0;

  constexpr static std::size_t min_memory_block_size() { return sizeof(node); }
  constexpr static std::size_t min_memory_block_align() { return alignof(node); }

  /// Pushes a new memory block to the linked list.
  void push(void* block) noexcept
  {
    srsran_assert(is_aligned(block, min_memory_block_align()), "The provided memory block is not aligned");
    node* ptr = ::new (block) node(head);
    head      = ptr;
    count++;
  }

  /// Pops a memory block from the linked list.
  void* pop() noexcept
  {
    srsran_assert(not empty(), "pop() called on empty list");
    node* last_head = head;
    head            = head->next;
    last_head->~node();
    count--;
    return static_cast<void*>(last_head);
  }

  void* try_pop() noexcept { return empty() ? nullptr : pop(); }

  /// Steals the top pointer from another memory block list.
  void steal_top(intrusive_memory_block_list& other) noexcept
  {
    srsran_assert(not other.empty(), "Trying to steal from empty memory_block list");
    node* other_head = other.head;
    other.head       = other.head->next;
    other_head->next = head;
    head             = other_head;
    other.count--;
    count++;
  }

  intrusive_memory_block_list try_pop_list(unsigned n)
  {
    intrusive_memory_block_list new_list{};
    if (n >= size()) {
      new_list.head  = head;
      new_list.count = count;
      head           = nullptr;
      count          = 0;
    } else if (n > 0) {
      node* prev = head;
      for (unsigned i = 0; i != n - 1; ++i) {
        prev = prev->next;
      }
      new_list.head  = head;
      new_list.count = n;
      head           = prev->next;
      count -= n;
      prev->next = nullptr;
    }
    return new_list;
  }

  void steal_blocks(intrusive_memory_block_list& list)
  {
    if (list.size() == 0) {
      return;
    }
    if (size() == 0) {
      std::swap(list, *this);
      return;
    }
    if (list.size() > size()) {
      node* prev = head;
      while (prev->next != nullptr) {
        prev = prev->next;
      }
      prev->next = list.head;
    } else {
      node* prev = list.head;
      while (prev->next != nullptr) {
        prev = prev->next;
      }
      prev->next = head;
      head       = list.head;
    }
    count += list.count;
    list.head  = nullptr;
    list.count = 0;
  }

  bool empty() const noexcept { return head == nullptr; }

  std::size_t size() const { return count; }

  void clear() noexcept
  {
    head  = nullptr;
    count = 0;
  }
};

} // namespace detail

/// \brief List of memory blocks. It overwrites bytes of blocks passed via push(void*). Thus, it is not safe to use
/// for any pool of initialized objects.
using free_memory_block_list = detail::intrusive_memory_block_list;

/// memory_block stack that mutexes pushing/popping
class concurrent_free_memory_block_list
{
public:
  concurrent_free_memory_block_list()                                         = default;
  concurrent_free_memory_block_list(const concurrent_free_memory_block_list&) = delete;
  concurrent_free_memory_block_list(concurrent_free_memory_block_list&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = other.stack;
  }
  concurrent_free_memory_block_list& operator=(const concurrent_free_memory_block_list&) = delete;
  concurrent_free_memory_block_list& operator=(concurrent_free_memory_block_list&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = other.stack;
    return *this;
  }

  void push(void* block) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.push(block);
  }

  void steal_blocks(free_memory_block_list& other) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.steal_blocks(other);
  }

  void steal_blocks(free_memory_block_list& other, size_t max_n) noexcept
  {
    if (max_n >= other.size()) {
      steal_blocks(other);
      return;
    }
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < max_n; ++i) {
      stack.push(other.try_pop());
    }
  }

  void* try_pop() noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    void*                       block = stack.try_pop();
    return block;
  }

  free_memory_block_list try_pop_list(unsigned n) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    return stack.try_pop_list(n);
  }

  template <size_t N>
  size_t try_pop(std::array<void*, N>& result) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    size_t                      i = 0;
    for (; i < N; ++i) {
      result[i] = stack.try_pop();
      if (result[i] == nullptr) {
        break;
      }
    }
    return i;
  }

  bool empty() const noexcept { return stack.empty(); }

  size_t size() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    return stack.size();
  }

  void clear()
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.clear();
  }

private:
  free_memory_block_list stack;
  mutable std::mutex     mutex;
};

} // namespace srsran