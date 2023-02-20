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
#include "srsgnb/support/srsgnb_assert.h"
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
    srsgnb_assert(is_aligned(block, min_memory_block_align()), "The provided memory block is not aligned");
    node* ptr = ::new (block) node(head);
    head      = ptr;
    count++;
  }

  /// Pops a memory block from the linked list.
  void* pop() noexcept
  {
    srsgnb_assert(not empty(), "pop() called on empty list");
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
    srsgnb_assert(not other.empty(), "Trying to steal from empty memory_block list");
    node* other_head = other.head;
    other.head       = other.head->next;
    other_head->next = head;
    head             = other_head;
    other.count--;
    count++;
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
class free_memory_block_list : public detail::intrusive_memory_block_list
{
private:
  using base_t = detail::intrusive_memory_block_list;
  using base_t::count;
  using base_t::head;
};

/// \brief List of memory blocks, each memory block containing a header, where the next pointer is stored, and payload
/// where an initialized object is stored. Contrarily to \c free_memory_block_list, this list is safe to use with
/// initialized nodes.
/// Memory Structure:
/// memory block 1     memory block 2
/// [ next | node ]   [ next | node ]
///    '--------------^  '-----------> nullptr
class memory_block_node_list : public detail::intrusive_memory_block_list
{
  using base_t = detail::intrusive_memory_block_list;
  using base_t::count;
  using base_t::head;
  using base_t::try_pop;

public:
  const size_t memory_block_alignment;
  const size_t header_size;
  const size_t payload_size;
  const size_t memory_block_size;

  explicit memory_block_node_list(size_t node_size_, size_t node_alignment_ = alignof(std::max_align_t)) :
    memory_block_alignment(std::max(free_memory_block_list::min_memory_block_align(), node_alignment_)),
    header_size(align_next(base_t::min_memory_block_size(), memory_block_alignment)),
    payload_size(align_next(node_size_, memory_block_alignment)),
    memory_block_size(header_size + payload_size)
  {
    srsgnb_assert(node_size_ > 0 and is_alignment_valid(node_alignment_),
                  "Invalid arguments node size={}, alignment={}",
                  node_size_,
                  node_alignment_);
  }

  void* get_node_header(void* payload_addr)
  {
    srsgnb_assert(is_aligned(payload_addr, memory_block_alignment), "Provided address is not valid");
    return static_cast<void*>(static_cast<uint8_t*>(payload_addr) - header_size);
  }

  /// returns address of memory_block payload (skips memory_block header).
  void* top() noexcept { return static_cast<void*>(reinterpret_cast<uint8_t*>(this->head) + header_size); }
};

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

  void steal_blocks(free_memory_block_list& other, size_t max_n) noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < max_n and not other.empty(); ++i) {
      stack.push(other.try_pop());
    }
  }

  void* try_pop() noexcept
  {
    std::lock_guard<std::mutex> lock(mutex);
    void*                       block = stack.try_pop();
    return block;
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