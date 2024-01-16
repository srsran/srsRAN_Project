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

#include "srsran/adt/detail/byte_buffer_segment.h"
#include <utility>

namespace srsran {
namespace detail {

/// Forward Linked list of byte buffer segments.
class byte_buffer_segment_list
{
public:
  /// \brief Node of the linked list containing a byte_buffer_segment and next pointer.
  struct node_t : public byte_buffer_segment {
    node_t* next = nullptr;

    using byte_buffer_segment::byte_buffer_segment;
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

/// This class implements a byte iterator for a byte_buffer_segment_list.
///
/// \tparam T Either uint8_t or const uint8_t.
template <typename T>
class byte_buffer_segment_byte_iterator_impl
{
public:
  using iterator_type     = byte_buffer_segment_byte_iterator_impl<T>;
  using value_type        = std::remove_const_t<T>;
  using reference         = T&;
  using pointer           = T*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  byte_buffer_segment_byte_iterator_impl() = default;
  byte_buffer_segment_byte_iterator_impl(byte_buffer_segment_list::node_t* start_segment) :
    current_segment(start_segment)
  {
  }
  byte_buffer_segment_byte_iterator_impl(byte_buffer_segment_list::node_t* start_segment, size_t offset_) :
    current_segment(start_segment), offset(offset_)
  {
  }

  /// Conversion from iterators of uint8_t (iterator) to const uint8_t (const_iterator).
  template <typename U, std::enable_if_t<not std::is_same<U, T>::value, bool> = true>
  byte_buffer_segment_byte_iterator_impl(const byte_buffer_segment_byte_iterator_impl<U>& other) :
    current_segment(other.current_segment), offset(other.offset)
  {
  }

  reference operator*() { return *(current_segment->data() + offset); }
  reference operator*() const { return *(current_segment->data() + offset); }
  pointer   operator->() { return (current_segment->data() + offset); }
  pointer   operator->() const { return (current_segment->data() + offset); }

  byte_buffer_segment_byte_iterator_impl& operator++()
  {
    offset++;
    if (offset >= current_segment->length()) {
      offset          = 0;
      current_segment = current_segment->next;
    }
    return *this;
  }
  byte_buffer_segment_byte_iterator_impl operator++(int)
  {
    byte_buffer_segment_byte_iterator_impl tmp(*this);
    ++(*this);
    return tmp;
  }
  byte_buffer_segment_byte_iterator_impl operator+(unsigned n) const
  {
    byte_buffer_segment_byte_iterator_impl tmp(*this);
    tmp += n;
    return tmp;
  }
  byte_buffer_segment_byte_iterator_impl& operator+=(unsigned n)
  {
    offset += n;
    while (current_segment != nullptr and offset >= current_segment->length()) {
      offset -= current_segment->length();
      current_segment = current_segment->next;
    }
    srsran_assert(current_segment != nullptr or offset == 0, "Out-of-bounds Access");
    return *this;
  }

  /// Distance between two iterators. The iterators must point at the same list of segments.
  difference_type operator-(const byte_buffer_segment_byte_iterator_impl<T>& other) const
  {
    difference_type                   diff = 0;
    byte_buffer_segment_list::node_t* seg  = other.current_segment;
    for (; seg != current_segment; seg = seg->next) {
      diff += seg->length();
    }
    diff += offset - other.offset;
    return diff;
  }

  bool operator==(const byte_buffer_segment_byte_iterator_impl<T>& other) const
  {
    return current_segment == other.current_segment and offset == other.offset;
  }
  bool operator!=(const byte_buffer_segment_byte_iterator_impl<T>& other) const { return !(*this == other); }

private:
  template <typename OtherT>
  friend class byte_buffer_segment_byte_iterator_impl;
  template <typename U>
  friend class byte_buffer_segment_list_span_iterator_impl;

  byte_buffer_segment_list::node_t* current_segment = nullptr;
  size_t                            offset          = 0;
};

using byte_buffer_segment_list_byte_iterator       = byte_buffer_segment_byte_iterator_impl<uint8_t>;
using byte_buffer_segment_list_byte_const_iterator = byte_buffer_segment_byte_iterator_impl<const uint8_t>;

/// \brief This class implements a span iterator for a byte_buffer_segment_list. Each span is a reference to the
/// payload of a byte_buffer_segment of the byte_buffer_segment_list.
template <typename NodeSegmentType>
class byte_buffer_segment_list_span_iterator_impl
{
  using byte_type = std::conditional_t<std::is_const<NodeSegmentType>::value, const uint8_t, uint8_t>;

public:
  using iterator_type     = byte_buffer_segment_list_span_iterator_impl<NodeSegmentType>;
  using value_type        = span<byte_type>;
  using reference         = span<byte_type>;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  struct pointer {
    pointer(span<byte_type> ar_) : ar(ar_) {}
    span<byte_type>*       operator->() { return &ar; }
    const span<byte_type>* operator->() const { return &ar; }

  private:
    span<byte_type> ar;
  };

  /// Create an iterator given a node of a byte_buffer_segment_list, an offset and a total range size in bytes.
  byte_buffer_segment_list_span_iterator_impl(NodeSegmentType* seg, size_t offset_, size_t size_) :
    current_segment(seg), offset(offset_), rem_bytes(size_)
  {
    srsran_assert(current_segment != nullptr or (offset == 0 and rem_bytes == 0),
                  "Positive offset or length for empty segment");
    srsran_assert(current_segment == nullptr or offset <= current_segment->length(), "Invalid offset");
  }

  template <typename U>
  byte_buffer_segment_list_span_iterator_impl(const byte_buffer_segment_byte_iterator_impl<U>& it, size_t size_) :
    byte_buffer_segment_list_span_iterator_impl(it.current_segment, it.offset, size_)
  {
  }

  /// Copy constructor.
  template <typename OtherT>
  byte_buffer_segment_list_span_iterator_impl(const byte_buffer_segment_list_span_iterator_impl<OtherT>& other) noexcept
    :
    byte_buffer_segment_list_span_iterator_impl(other.current_segment, other.offset, other.rem_bytes)
  {
  }

  /// Access span of bytes representing the payload of the current segment.
  reference operator*()
  {
    return current_segment != nullptr ? reference{current_segment->data() + offset,
                                                  std::min(rem_bytes, (unsigned)current_segment->length() - offset)}
                                      : reference{};
  }
  reference operator*() const
  {
    return current_segment != nullptr ? reference{current_segment->data() + offset,
                                                  std::min(rem_bytes, (unsigned)current_segment->length() - offset)}
                                      : reference{};
  }
  pointer operator->() { return pointer{this->operator*()}; }
  pointer operator->() const { return pointer{this->operator*()}; }

  iterator_type& operator++()
  {
    srsran_assert(current_segment != nullptr, "operator++ called after end()");
    rem_bytes -= std::min(rem_bytes, (unsigned)current_segment->length() - offset);
    offset = 0;
    if (rem_bytes == 0) {
      current_segment = nullptr;
    } else {
      current_segment = current_segment->next;
    }
    return *this;
  }
  iterator_type operator++(int)
  {
    iterator_type tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator==(const iterator_type& other) const
  {
    return current_segment == other.current_segment and offset == other.offset;
  }
  bool operator!=(const iterator_type& other) const { return !(*this == other); }

private:
  template <typename OtherT>
  friend class byte_buffer_segment_list_span_iterator_impl;

  NodeSegmentType* current_segment = nullptr;
  unsigned         offset          = 0;
  unsigned         rem_bytes       = 0;
};

/// \brief This class implements a range (a begin and end pair) of byte_buffer_segment_list span iterators.
/// Each element of the range corresponds to a span of bytes, which, in turn, points to the payload of an underlying
/// byte_buffer_segment.
template <typename SegmentNodeType>
class byte_buffer_segment_list_span_range_impl
{
public:
  using iterator       = byte_buffer_segment_list_span_iterator_impl<SegmentNodeType>;
  using const_iterator = byte_buffer_segment_list_span_iterator_impl<const SegmentNodeType>;

  byte_buffer_segment_list_span_range_impl(SegmentNodeType* seg, size_t offset, size_t size) : begin_(seg, offset, size)
  {
  }
  template <typename U>
  byte_buffer_segment_list_span_range_impl(const byte_buffer_segment_byte_iterator_impl<U>& it, size_t size) :
    begin_(it, size)
  {
  }

  iterator       begin() { return begin_; }
  const_iterator begin() const { return begin_; }
  iterator       end() { return iterator(nullptr, 0, 0); }
  const_iterator end() const { return const_iterator(nullptr, 0, 0); }

  bool empty() const { return begin() == end(); }

private:
  iterator begin_;
};

} // namespace detail

/// Range of byte spans belonging to a byte_buffer_segment_list.
using byte_buffer_segment_span_range =
    detail::byte_buffer_segment_list_span_range_impl<detail::byte_buffer_segment_list::node_t>;
using const_byte_buffer_segment_span_range =
    detail::byte_buffer_segment_list_span_range_impl<const detail::byte_buffer_segment_list::node_t>;

/// \brief Checks whether a type represents a range of byte_buffer_iterators (e.g. byte_buffer, byte_buffer_slice,
/// byte_buffer_view).
template <typename ByteBufferType>
using is_byte_buffer_range =
    std::is_same<typename ByteBufferType::iterator, detail::byte_buffer_segment_list_byte_iterator>;

} // namespace srsran
