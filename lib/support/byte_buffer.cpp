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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/detail/byte_buffer_segment_pool.h"
#include "srsran/support/memory_pool/linear_memory_allocator.h"

using namespace srsran;

size_t srsran::byte_buffer_segment_pool_default_segment_size()
{
  return 2048;
}

/// Get default byte buffer segment pool. Initialize pool if not initialized before.
detail::byte_buffer_segment_pool& srsran::detail::get_default_byte_buffer_segment_pool()
{
  // Initialize byte buffer segment pool, if not yet initialized.
  // Note: In case of unit tests, this function will be called rather than init_byte_buffer_segment_pool(...).
  constexpr static size_t default_byte_buffer_segment_pool_size = 16384;
  static auto&            pool = detail::byte_buffer_segment_pool::get_instance(default_byte_buffer_segment_pool_size,
                                                                     byte_buffer_segment_pool_default_segment_size());
  return pool;
}

void srsran::init_byte_buffer_segment_pool(std::size_t nof_segments, std::size_t memory_block_size)
{
  auto& pool = detail::byte_buffer_segment_pool::get_instance(nof_segments, memory_block_size);
  report_fatal_error_if_not(nof_segments == pool.nof_memory_blocks(),
                            "The pool was already initialized with a different number of segments ({} != {})",
                            nof_segments,
                            pool.nof_memory_blocks());
  report_fatal_error_if_not(memory_block_size > 64U, "memory blocks must be larger than the segment control header");
}

// ------- byte_buffer class -------

void byte_buffer::control_block::destroy_node(node_t* node) const
{
  node->~node_t();
  if (node != segment_in_cb_memory_block) {
    if (not this->malloc_fallback or detail::byte_buffer_segment_pool::get_instance().owns_segment(node)) {
      detail::byte_buffer_segment_pool::get_instance().deallocate_node(node);
    } else {
      delete[] reinterpret_cast<uint8_t*>(node);
    }
  }
}

byte_buffer::control_block::~control_block()
{
  // Destroy and return all segments back to the segment memory pool.
  for (node_t *next_node = segments.head, *node = next_node; node != nullptr; node = next_node) {
    next_node = node->next;
    destroy_node(node);
  }
}

void byte_buffer::control_block::destroy_cb()
{
  bool pool_used = not this->malloc_fallback or detail::byte_buffer_segment_pool::get_instance().owns_segment(this);
  this->~control_block();
  if (pool_used) {
    detail::get_default_byte_buffer_segment_pool().deallocate_node(this);
  } else {
    delete[] reinterpret_cast<uint8_t*>(this);
  }
}

// ----- byte_buffer -----

byte_buffer::byte_buffer(fallback_allocation_tag tag, span<const uint8_t> other) noexcept
{
  // Append new head segment to linked list with fallback allocator mode.
  node_t* n = add_head_segment(DEFAULT_FIRST_SEGMENT_HEADROOM, true);
  srsran_sanity_check(n != nullptr, "Should never fail to append segment if fallback is enabled");

  bool var = this->append(other);
  srsran_sanity_check(var, "Should never fail to append segment if fallback is enabled");
  (void)var;
}

byte_buffer::byte_buffer(fallback_allocation_tag tag, const byte_buffer& other) noexcept
{
  // Append new head segment to linked list with fallback allocator mode.
  node_t* n = add_head_segment(DEFAULT_FIRST_SEGMENT_HEADROOM, true);
  srsran_sanity_check(n != nullptr, "Should never fail to append segment if fallback is enabled");

  for (span<const uint8_t> seg : other.segments()) {
    bool var = this->append(seg);
    srsran_sanity_check(var, "Should never fail to append segment if fallback is enabled");
    (void)var;
  }
}

bool byte_buffer::append(span<const uint8_t> bytes)
{
  if (bytes.empty()) {
    // no bytes to append.
    return true;
  }
  if (not has_ctrl_block() and not append_segment(DEFAULT_FIRST_SEGMENT_HEADROOM)) {
    // failed to allocate head segment.
    return false;
  }

  // segment-wise copy.
  for (size_t count = 0; count < bytes.size();) {
    if (ctrl_blk_ptr->segments.tail->tailroom() == 0 and not append_segment(0)) {
      return false;
    }
    size_t              to_write = std::min(ctrl_blk_ptr->segments.tail->tailroom(), bytes.size() - count);
    span<const uint8_t> subspan  = bytes.subspan(count, to_write);
    ctrl_blk_ptr->segments.tail->append(subspan);
    count += to_write;
    ctrl_blk_ptr->pkt_len += to_write;
  }
  return true;
}

bool byte_buffer::append(const byte_buffer& other)
{
  srsran_assert(&other != this, "Self-append not supported");
  if (other.empty()) {
    return true;
  }
  if (not has_ctrl_block() and not append_segment(other.ctrl_blk_ptr->segments.head->headroom())) {
    return false;
  }
  for (node_t* seg = other.ctrl_blk_ptr->segments.head; seg != nullptr; seg = seg->next) {
    auto* other_it = seg->begin();
    while (other_it != seg->end()) {
      if (ctrl_blk_ptr->segments.tail->tailroom() == 0 and not append_segment(0)) {
        return false;
      }
      auto to_append =
          std::min(seg->end() - other_it, (iterator::difference_type)ctrl_blk_ptr->segments.tail->tailroom());
      ctrl_blk_ptr->segments.tail->append(other_it, other_it + to_append);
      other_it += to_append;
    }
    ctrl_blk_ptr->pkt_len += seg->length();
  }
  return true;
}

bool byte_buffer::append(byte_buffer&& other)
{
  srsran_assert(&other != this, "Self-append not supported");
  if (other.empty()) {
    return true;
  }
  if (empty()) {
    *this = std::move(other);
    return true;
  }
  if (not other.ctrl_blk_ptr->ref_count.unique()) {
    // Use lvalue append.
    return append(other);
  }
  // This is the last reference to "after". Shallow copy, except control segment.
  node_t* node = create_segment(0);
  if (node == nullptr) {
    return false;
  }
  node->append(span<uint8_t>{other.ctrl_blk_ptr->segment_in_cb_memory_block->data(),
                             other.ctrl_blk_ptr->segment_in_cb_memory_block->length()});
  ctrl_blk_ptr->pkt_len += other.ctrl_blk_ptr->pkt_len;
  node_t* last_tail           = ctrl_blk_ptr->segments.tail;
  last_tail->next             = other.ctrl_blk_ptr->segments.head;
  ctrl_blk_ptr->segments.tail = other.ctrl_blk_ptr->segments.tail;
  node->next                  = other.ctrl_blk_ptr->segment_in_cb_memory_block->next;
  if (other.ctrl_blk_ptr->segment_in_cb_memory_block == other.ctrl_blk_ptr->segments.tail) {
    ctrl_blk_ptr->segments.tail = node;
  }
  for (node_t* seg = last_tail; seg->next != nullptr; seg = seg->next) {
    if (seg->next == other.ctrl_blk_ptr->segment_in_cb_memory_block) {
      seg->next = node;
      break;
    }
  }
  other.ctrl_blk_ptr->segments.head       = other.ctrl_blk_ptr->segment_in_cb_memory_block;
  other.ctrl_blk_ptr->segments.tail       = other.ctrl_blk_ptr->segment_in_cb_memory_block;
  other.ctrl_blk_ptr->segments.head->next = nullptr;
  other.ctrl_blk_ptr.reset();
  return true;
}

byte_buffer::node_t* byte_buffer::add_head_segment(size_t headroom, bool use_fallback)
{
  auto&        pool       = detail::get_default_byte_buffer_segment_pool();
  const size_t block_size = pool.memory_block_size();

  // Allocate new node.
  void* mem_block = pool.allocate_node(block_size);
  if (mem_block == nullptr) {
    if (not use_fallback) {
      // Pool is depleted.
      byte_buffer::warn_alloc_failure();
      return nullptr;
    }
    // Use heap as fallback.
    mem_block = new uint8_t[block_size];
  }

  // Construct linear allocator pointing to allocated segment memory block.
  linear_memory_allocator arena{mem_block, block_size};

  // Create control block using allocator.
  void* cb_region = arena.allocate(sizeof(control_block), alignof(control_block));
  ctrl_blk_ptr    = new (cb_region) control_block{};
  srsran_sanity_check(ctrl_blk_ptr != nullptr, "Something went wrong with the creation of the control block");
  ctrl_blk_ptr->malloc_fallback = use_fallback;

  // For first segment of byte_buffer, add a headroom.
  void*   segment_header_region = arena.allocate(sizeof(node_t), alignof(node_t));
  size_t  segment_size          = arena.nof_bytes_left();
  void*   payload_start         = arena.allocate(segment_size, 1);
  node_t* node                  = new (segment_header_region)
      node_t(span<uint8_t>{static_cast<uint8_t*>(payload_start), segment_size}, std::min(headroom, segment_size));
  srsran_sanity_check(node != nullptr, "Something went wrong with the creation of the segment");

  // Register segment as sharing the same memory block with control block.
  ctrl_blk_ptr->segment_in_cb_memory_block = node;

  // Append new segment to linked list.
  ctrl_blk_ptr->segments.push_back(*node);

  return node;
}

byte_buffer::node_t* byte_buffer::create_segment(size_t headroom)
{
  auto&        pool       = detail::get_default_byte_buffer_segment_pool();
  const size_t block_size = pool.memory_block_size();

  // Allocate memory block.
  void* mem_block = pool.allocate_node(block_size);
  if (mem_block == nullptr) {
    if (not ctrl_blk_ptr->malloc_fallback) {
      byte_buffer::warn_alloc_failure();
      return nullptr;
    }
    // Use malloc as fallback.
    mem_block = new uint8_t[block_size];
  }

  // Create a linear allocator pointing to the allocated memory block.
  linear_memory_allocator arena{mem_block, block_size};

  void*  segment_start = arena.allocate(sizeof(node_t), alignof(node_t));
  size_t segment_size  = arena.nof_bytes_left();
  void*  payload_start = arena.allocate(segment_size, 1);
  return new (segment_start)
      node_t(span<uint8_t>{static_cast<uint8_t*>(payload_start), segment_size}, std::min(headroom, segment_size));
}

bool byte_buffer::append_segment(size_t headroom_suggestion)
{
  if (not has_ctrl_block()) {
    return add_head_segment(headroom_suggestion) != nullptr;
  }
  node_t* segment = create_segment(headroom_suggestion);
  if (segment == nullptr) {
    return false;
  }
  // Append new segment to linked list.
  ctrl_blk_ptr->segments.push_back(*segment);
  return true;
}

bool byte_buffer::prepend_segment(size_t headroom_suggestion)
{
  if (not has_ctrl_block()) {
    return add_head_segment(headroom_suggestion) != nullptr;
  }
  node_t* segment = create_segment(headroom_suggestion);
  if (segment == nullptr) {
    return false;
  }
  // Prepend new segment to linked list.
  ctrl_blk_ptr->segments.push_front(*segment);
  return true;
}

bool byte_buffer::prepend(span<const uint8_t> bytes)
{
  if (empty()) {
    // the byte buffer is empty. Prepending is the same as appending.
    return append(bytes);
  }
  for (size_t count = 0; count < bytes.size();) {
    if (ctrl_blk_ptr->segments.head->headroom() == 0) {
      if (not prepend_segment(bytes.size() - count)) {
        return false;
      }
    }
    size_t              to_write = std::min(ctrl_blk_ptr->segments.head->headroom(), bytes.size() - count);
    span<const uint8_t> subspan  = bytes.subspan(bytes.size() - to_write - count, to_write);
    ctrl_blk_ptr->segments.head->prepend(subspan);
    ctrl_blk_ptr->pkt_len += to_write;
    count += to_write;
  }
  return true;
}

bool byte_buffer::prepend(const byte_buffer& other)
{
  srsran_assert(&other != this, "Self-append not supported");
  if (other.empty()) {
    return true;
  }
  if (empty()) {
    // the byte buffer is empty. Prepending is the same as appending.
    return append(other);
  }
  for (span<const uint8_t> seg : other.segments()) {
    node_t* node = create_segment(0);
    if (node == nullptr) {
      return false;
    }
    node->append(seg);
    node->next                  = ctrl_blk_ptr->segments.head;
    ctrl_blk_ptr->segments.head = node;
    ctrl_blk_ptr->pkt_len += seg.size();
  }
  return true;
}

bool byte_buffer::prepend(byte_buffer&& other)
{
  srsran_assert(&other != this, "Self-append not supported");
  if (other.empty()) {
    return true;
  }
  if (empty()) {
    // the byte buffer is empty. Prepending is the same as appending.
    return append(std::move(other));
  }
  if (not other.ctrl_blk_ptr->ref_count.unique()) {
    // Deep copy of segments.
    return prepend(other);
  }

  // This is the last reference to "other". Shallow copy, except control segment.
  node_t* node = create_segment(0);
  if (node == nullptr) {
    return false;
  }
  node->append(span<uint8_t>{other.ctrl_blk_ptr->segment_in_cb_memory_block->data(),
                             other.ctrl_blk_ptr->segment_in_cb_memory_block->length()});
  ctrl_blk_ptr->pkt_len += other.ctrl_blk_ptr->pkt_len;
  other.ctrl_blk_ptr->segments.tail->next = ctrl_blk_ptr->segments.head;
  node->next                              = other.ctrl_blk_ptr->segment_in_cb_memory_block->next;
  if (other.ctrl_blk_ptr->segment_in_cb_memory_block == other.ctrl_blk_ptr->segments.head) {
    ctrl_blk_ptr->segments.head = node;
  } else {
    for (node_t* seg = other.ctrl_blk_ptr->segments.head; seg->next != nullptr; seg = seg->next) {
      if (seg->next == other.ctrl_blk_ptr->segment_in_cb_memory_block) {
        seg->next = node;
        break;
      }
    }
  }
  other.ctrl_blk_ptr->segments.head       = other.ctrl_blk_ptr->segment_in_cb_memory_block;
  other.ctrl_blk_ptr->segments.tail       = other.ctrl_blk_ptr->segment_in_cb_memory_block;
  other.ctrl_blk_ptr->segments.head->next = nullptr;
  other.ctrl_blk_ptr.reset();
  return true;
}

byte_buffer_view byte_buffer::reserve_prepend(size_t nof_bytes)
{
  size_t rem_bytes = nof_bytes;
  while (rem_bytes > 0) {
    if (empty() or ctrl_blk_ptr->segments.head->headroom() == 0) {
      if (not prepend_segment(rem_bytes)) {
        return {};
      }
    }
    size_t to_reserve = std::min(ctrl_blk_ptr->segments.head->headroom(), rem_bytes);
    ctrl_blk_ptr->segments.head->reserve_prepend(to_reserve);
    rem_bytes -= to_reserve;
  }
  ctrl_blk_ptr->pkt_len += nof_bytes;
  return byte_buffer_view{begin(), begin() + nof_bytes};
}

void byte_buffer::trim_head(size_t nof_bytes)
{
  srsran_assert(length() >= nof_bytes, "Trying to trim more bytes than those available");
  for (size_t trimmed = 0; trimmed != nof_bytes;) {
    size_t to_trim = std::min(nof_bytes - trimmed, ctrl_blk_ptr->segments.head->length());
    ctrl_blk_ptr->segments.head->trim_head(to_trim);
    ctrl_blk_ptr->pkt_len -= to_trim;
    trimmed += to_trim;
    if (ctrl_blk_ptr->segments.head->length() == 0) {
      // Head segment is empty.
      // Remove the first segment.
      node_t* prev_head           = ctrl_blk_ptr->segments.head;
      ctrl_blk_ptr->segments.head = ctrl_blk_ptr->segments.head->next;
      ctrl_blk_ptr->destroy_node(prev_head);
    }
  }
}

void byte_buffer::trim_tail(size_t nof_bytes)
{
  srsran_assert(length() >= nof_bytes, "Trimming too many bytes from byte_buffer");
  if (nof_bytes == 0) {
    return;
  }

  if (ctrl_blk_ptr->segments.tail->length() >= nof_bytes) {
    // Simplest scenario where the last segment is larger than the number of bytes to trim.
    ctrl_blk_ptr->segments.tail->trim_tail(nof_bytes);
    ctrl_blk_ptr->pkt_len -= nof_bytes;
    if (ctrl_blk_ptr->segments.tail->length() == 0) {
      pop_last_segment();
    }
    return;
  }
  size_t  new_len = length() - nof_bytes;
  node_t* seg     = ctrl_blk_ptr->segments.head;
  for (size_t count = 0; seg != nullptr; seg = seg->next) {
    if (count + seg->length() >= new_len) {
      // We reached the new last segment.
      // Destroy remaining segments.
      for (auto *seg_next = seg->next, *to_destroy = seg_next; to_destroy != nullptr; to_destroy = seg_next) {
        seg_next = to_destroy->next;
        ctrl_blk_ptr->destroy_node(to_destroy);
      }

      // Set this node as tail.
      seg->next = nullptr;
      seg->resize(new_len - count);
      ctrl_blk_ptr->segments.tail = seg;
      ctrl_blk_ptr->pkt_len       = new_len;
      break;
    }
    count += seg->length();
  }
}

bool byte_buffer::linearize()
{
  if (is_contiguous()) {
    return true;
  }
  size_t sz = length();
  if (sz > ctrl_blk_ptr->segments.head->capacity() - ctrl_blk_ptr->segments.head->headroom()) {
    return false;
  }
  for (node_t* seg = ctrl_blk_ptr->segments.head->next; seg != nullptr;) {
    node_t* next = seg->next;
    ctrl_blk_ptr->segments.head->append(seg->begin(), seg->end());
    ctrl_blk_ptr->destroy_node(seg);
    seg = next;
  }
  ctrl_blk_ptr->segments.head->next = nullptr;
  ctrl_blk_ptr->segments.tail       = ctrl_blk_ptr->segments.head;
  return true;
}

bool byte_buffer::resize(size_t new_sz)
{
  size_t prev_len = length();
  if (new_sz == prev_len) {
    return true;
  }
  if (new_sz > prev_len) {
    for (size_t to_add = new_sz - prev_len; to_add > 0;) {
      if (not has_ctrl_block() or ctrl_blk_ptr->segments.tail->tailroom() == 0) {
        if (not append_segment(0)) {
          return false;
        }
      }
      size_t added = std::min(ctrl_blk_ptr->segments.tail->tailroom(), to_add);
      ctrl_blk_ptr->segments.tail->resize(added);
      to_add -= added;
    }
  } else {
    size_t count = 0;
    for (node_t* seg = ctrl_blk_ptr->segments.head; count < new_sz; seg = seg->next) {
      size_t new_seg_len = std::min(seg->length(), new_sz - count);
      if (new_seg_len != seg->length()) {
        seg->resize(new_seg_len);
      }
      count += new_seg_len;
      if (count == new_sz) {
        seg->next                   = nullptr;
        ctrl_blk_ptr->segments.tail = seg;
      }
    }
  }
  ctrl_blk_ptr->pkt_len = new_sz;
  return true;
}

void byte_buffer::warn_alloc_failure()
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
  logger.warning("POOL: Failure to allocate byte buffer segment");
}
