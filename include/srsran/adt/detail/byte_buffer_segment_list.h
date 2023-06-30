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

#include "srsran/adt/detail/byte_buffer_segment2.h"

namespace srsran {
namespace detail {

/// Forward Linked list of byte buffer segments.
class byte_buffer_segment_list
{
public:
  /// \brief Node of the linked list containing a byte_buffer_segment and next pointer.
  struct node_t : public byte_buffer_segment2 {
    node_t* next = nullptr;

    using byte_buffer_segment2::byte_buffer_segment2;
  };

  /// First segment of the linked list of segments.
  node_t* head = nullptr;

  /// Tail of linked list of segments. This value is != nullptr if the list is not empty.
  node_t* tail = nullptr;

  /// Creats an empty linked list of segments.
  byte_buffer_segment_list() = default;

  /// Move constructor.
  byte_buffer_segment_list(byte_buffer_segment_list&& other) noexcept :
    head(std::exchange(other.head, nullptr)), tail(std::exchange(other.tail, nullptr))
  {
  }

  /// Move assignment operator.
  byte_buffer_segment_list& operator=(byte_buffer_segment_list&& other) noexcept
  {
    head = std::exchange(other.head, nullptr);
    tail = std::exchange(other.tail, nullptr);
    return *this;
  }

  /// Push a new node to the back of the linked list.
  void push_back(node_t& seg) noexcept
  {
    seg.next = nullptr;
    if (head != nullptr) {
      tail->next = &seg;
      tail       = &seg;
    } else {
      head = &seg;
      tail = &seg;
    }
  }

  /// Push a new node as the new head of the linked list.
  void push_front(node_t& seg)
  {
    seg.next = head;
    if (tail == nullptr) {
      tail = &seg;
    }
    head = &seg;
  }

  /// Pop a node from the front of the linked list. No-op if the list is empty.
  void pop_front()
  {
    if (head == tail) {
      head = nullptr;
      tail = nullptr;
    } else {
      head = head->next;
    }
  }

  /// Pop a node from the tail of the linked list. No-op if the list is empty. This operation has O(n) complexity.
  void pop_back()
  {
    if (head == tail) {
      head = nullptr;
      tail = nullptr;
      return;
    }

    // tail needs to be recomputed.
    node_t* new_tail = head;
    for (; new_tail->next != tail; new_tail = new_tail->next) {
    }
    tail       = new_tail;
    tail->next = nullptr;
  }

  /// Checks whether the linked list is empty.
  bool empty() const { return head == nullptr; }

  /// First node of the linked list.
  node_t&       front() { return *head; }
  const node_t& front() const { return *head; }

  /// Last node of the linked list.
  node_t&       back() { return *tail; }
  const node_t& back() const { return *tail; }
};

} // namespace detail
} // namespace srsran