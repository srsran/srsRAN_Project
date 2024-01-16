/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "memory_pool_utils.h"
#include "srsran/support/srsran_assert.h"
#include <mutex>
#include <utility>

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
  node*       tail  = nullptr;
  std::size_t count = 0;

  constexpr static std::size_t min_memory_block_size() { return sizeof(node); }
  constexpr static std::size_t min_memory_block_align() { return alignof(node); }

  /// Pushes a new memory block to the linked list.
  void push(void* block) noexcept
  {
    srsran_assert(is_aligned(block, min_memory_block_align()), "The provided memory block is not aligned");
    node* ptr = ::new (block) node(head);
    head      = ptr;
    if (tail == nullptr) {
      tail = ptr;
    }
    count++;
  }

  /// Pops a memory block from the linked list.
  void* pop() noexcept
  {
    srsran_assert(not empty(), "pop() called on empty list");
    node* last_head = head;
    head            = head->next;
    if (head == nullptr) {
      tail = nullptr;
    }
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
    if (other.head == nullptr) {
      other.tail = nullptr;
    }
    other_head->next = head;
    head             = other_head;
    if (tail == nullptr) {
      head = tail;
    }
    other.count--;
    count++;
  }

  intrusive_memory_block_list try_pop_batch(unsigned n)
  {
    intrusive_memory_block_list new_list{};
    if (n >= size()) {
      new_list.head  = std::exchange(head, nullptr);
      new_list.tail  = std::exchange(tail, nullptr);
      new_list.count = std::exchange(count, 0);
    } else if (n > 0) {
      node* prev = head;
      for (unsigned i = 0; i != n - 1; ++i) {
        prev = prev->next;
      }
      new_list.head  = head;
      new_list.tail  = prev;
      new_list.count = n;
      head           = std::exchange(prev->next, nullptr);
      count -= n;
    }
    return new_list;
  }

  void steal_blocks(intrusive_memory_block_list& list)
  {
    if (list.empty()) {
      return;
    }
    if (empty()) {
      head = std::exchange(list.head, nullptr);
    } else {
      tail->next = std::exchange(list.head, nullptr);
    }
    tail = std::exchange(list.tail, nullptr);
    count += std::exchange(list.count, 0);
  }

  bool empty() const noexcept { return head == nullptr; }

  std::size_t size() const { return count; }

  void clear() noexcept
  {
    head  = nullptr;
    tail  = nullptr;
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
    return stack.try_pop_batch(n);
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