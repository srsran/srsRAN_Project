/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/linear_memory_allocator.h"

using namespace srsran;

static constexpr size_t default_pool_nof_segments = 16384;
static constexpr size_t default_segment_size      = 2048;

size_t srsran::byte_buffer_segment_pool_default_segment_size()
{
  return default_segment_size;
}

/// Get default byte buffer segment pool. Initialize pool if not initialized before.
detail::byte_buffer_segment_pool& srsran::detail::get_default_byte_buffer_segment_pool()
{
  // Initialize byte buffer segment pool, if not yet initialized.
  // Note: In case of unit tests, this function will be called rather than init_byte_buffer_segment_pool(...).
  static auto& pool = detail::byte_buffer_segment_pool::get_instance(default_pool_nof_segments, default_segment_size);
  return pool;
}

void srsran::init_byte_buffer_segment_pool(std::size_t nof_segments, std::size_t memory_block_size)
{
  auto& pool = detail::byte_buffer_segment_pool::get_instance(nof_segments, memory_block_size);
  report_fatal_error_if_not(pool.nof_memory_blocks() >= nof_segments,
                            "The pool was already initialized with a lower number of segments ({} < {})",
                            pool.nof_memory_blocks(),
                            nof_segments);
  report_fatal_error_if_not(pool.memory_block_size() >= memory_block_size,
                            "The pool was already initialized with a smaller block size ({} < {})",
                            pool.memory_block_size(),
                            memory_block_size);
  report_fatal_error_if_not(memory_block_size > 64U, "memory blocks must be larger than the segment control header");
}

size_t srsran::get_byte_buffer_segment_pool_capacity()
{
  auto& pool = detail::get_default_byte_buffer_segment_pool();
  return pool.nof_memory_blocks();
}

size_t srsran::get_byte_buffer_segment_pool_current_size_approx()
{
  auto& pool = detail::get_default_byte_buffer_segment_pool();
  return pool.get_central_cache_approx_size() + pool.get_local_cache_size();
}

void srsran::init_byte_buffer_segment_pool_tls()
{
  detail::get_default_byte_buffer_segment_pool().init_worker_cache();
}

// ------ memory resource -------

/// Warn when the default segment pool is depleted.
static void byte_buffer_warn_alloc_failure()
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
  logger.warning("POOL: Failure to allocate byte buffer segment");
}

namespace {

/// Memory resource wrapper of the default byte buffer segment pool.
class default_segment_pool_memory_resource final : public byte_buffer_memory_resource
{
public:
  default_segment_pool_memory_resource(bool log_on_failure_) :
    log_on_failure(log_on_failure_), default_pool(detail::get_default_byte_buffer_segment_pool())
  {
  }

  bool owns_segment(void* block) const { return default_pool.owns_segment(block); }

private:
  span<uint8_t> do_allocate(size_t /* unused */, size_t /* unused */) override
  {
    // Fetch default pool.
    const size_t block_size = default_pool.memory_block_size();

    // Allocate new block.
    void* mem_block = default_pool.allocate_node(block_size);
    if (SRSRAN_LIKELY(mem_block != nullptr)) {
      // Allocation from pool was successful.
      return {static_cast<uint8_t*>(mem_block), block_size};
    }

    // Pool is depleted.
    if (log_on_failure) {
      byte_buffer_warn_alloc_failure();
    }
    return {};
  }

  void do_deallocate(void* block) override
  {
    // Return block back to the pool.
    default_pool.deallocate_node(block);
  }

  bool do_is_equal(const byte_buffer_memory_resource& other) const noexcept override { return this == &other; }

  const bool                        log_on_failure;
  detail::byte_buffer_segment_pool& default_pool;
};

/// \brief Memory resource wrapper of the default byte buffer segment pool, but that on pool depletion, performs as
/// a fallback an allocation on the heap with operator new.
class default_fallback_segment_pool_memory_resource final : public byte_buffer_memory_resource
{
public:
  default_fallback_segment_pool_memory_resource() : pool(false) {}

  span<uint8_t> do_allocate(size_t recommended_bytes, size_t alignment) override
  {
    // Allocate using the default segment pool.
    span<uint8_t> block = pool.allocate(recommended_bytes, alignment);

    if (block.empty()) {
      // Allocation failed. Resort to the heap as fallback.
      void* heap_block = new uint8_t[recommended_bytes];
      return {static_cast<uint8_t*>(heap_block), recommended_bytes};
    }

    return block;
  }

  void do_deallocate(void* block) override
  {
    if (pool.owns_segment(block)) {
      // The block comes from the default byte buffer pool.
      pool.deallocate(block);
      return;
    }

    // The block comes from the heap.
    delete[] static_cast<uint8_t*>(block);
  }

  bool do_is_equal(const byte_buffer_memory_resource& other) const noexcept override { return this == &other; }

private:
  default_segment_pool_memory_resource pool;
};

} // namespace

byte_buffer_memory_resource& srsran::get_default_byte_buffer_segment_pool()
{
  static default_segment_pool_memory_resource default_mem_resource{true};
  return default_mem_resource;
}

byte_buffer_memory_resource& srsran::get_default_fallback_byte_buffer_segment_pool()
{
  static default_fallback_segment_pool_memory_resource default_fallback_mem_resource;
  return default_fallback_mem_resource;
}

// ------- byte_buffer class -------

void byte_buffer::control_block::destroy_node(node_t* node) const
{
  void* block = node;
  node->~node_t();
  if (node != segment_in_cb_memory_block) {
    segment_pool->deallocate(block);
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
  void*                        block = this;
  byte_buffer_memory_resource* pool  = this->segment_pool;
  this->~control_block();
  pool->deallocate(block);
}

// ----- byte_buffer -----

expected<byte_buffer> byte_buffer::create(byte_buffer_memory_resource& segment_pool)
{
  byte_buffer result;
  if (not result.default_construct_unsafe(segment_pool,
                                          default_segment_size - sizeof(control_block) - sizeof(node_t))) {
    return make_unexpected(default_error_t{});
  }
  return result;
}

expected<byte_buffer> byte_buffer::create(span<const uint8_t> bytes, byte_buffer_memory_resource& segment_pool)
{
  byte_buffer result;
  if (not result.append(bytes, segment_pool)) {
    return make_unexpected(default_error_t{});
  }
  return result;
}

expected<byte_buffer> byte_buffer::create(const std::initializer_list<uint8_t>& lst, byte_buffer_memory_resource& pool)
{
  return create(span<const uint8_t>(lst.begin(), lst.size()), pool);
}

byte_buffer::byte_buffer(fallback_allocation_tag /* unused */, span<const uint8_t> other) noexcept
{
  // Create new byte buffer with default_fallback_pool.
  // Note: We can retrieve the value() directly as the default fallback pool is supposed to never fail.
  bool ret = append(other, get_default_fallback_byte_buffer_segment_pool());
  srsran_sanity_check(ret, "Should never fail to append segment if fallback is enabled");
}

byte_buffer::byte_buffer(fallback_allocation_tag tag, const std::initializer_list<uint8_t>& other) noexcept :
  byte_buffer(tag, span<const uint8_t>(other.begin(), other.end()))
{
}

byte_buffer::byte_buffer(fallback_allocation_tag /* unused */, const byte_buffer& other) noexcept
{
  // Append new head segment to linked list with fallback allocator mode.
  node_t* n = add_head_segment(DEFAULT_FIRST_SEGMENT_HEADROOM,
                               get_default_fallback_byte_buffer_segment_pool(),
                               other.length() + DEFAULT_FIRST_SEGMENT_HEADROOM);
  srsran_sanity_check(n != nullptr, "Should never fail to append segment if fallback is enabled");

  for (span<const uint8_t> seg : other.segments()) {
    bool var = this->append(seg);
    srsran_sanity_check(var, "Should never fail to append segment if fallback is enabled");
    (void)var;
  }
}

expected<byte_buffer> byte_buffer::deep_copy() const
{
  if (ctrl_blk_ptr == nullptr) {
    return byte_buffer{};
  }

  byte_buffer buf;
  for (node_t* seg = ctrl_blk_ptr->segments.head; seg != nullptr; seg = seg->next) {
    if (not buf.append(span<uint8_t>{seg->data(), seg->length()})) {
      return make_unexpected(default_error_t{});
    }
  }
  return buf;
}

expected<byte_buffer> byte_buffer::deep_copy(fallback_allocation_tag tag) const
{
  return byte_buffer{tag, *this};
}

bool byte_buffer::append(span<const uint8_t> bytes)
{
  return append(bytes, get_default_byte_buffer_segment_pool());
}

bool byte_buffer::append(const std::initializer_list<uint8_t>& bytes)
{
  return append(span<const uint8_t>(bytes.begin(), bytes.size()));
}

bool byte_buffer::append(const byte_buffer& other)
{
  srsran_assert(&other != this, "Self-append not supported");
  if (other.empty()) {
    return true;
  }
  if (not has_ctrl_block() and not append_segment(other.ctrl_blk_ptr->segments.head->headroom(),
                                                  other.ctrl_blk_ptr->segments.head->headroom() + other.length())) {
    return false;
  }
  for (node_t* seg = other.ctrl_blk_ptr->segments.head; seg != nullptr; seg = seg->next) {
    auto* other_it = seg->begin();
    while (other_it != seg->end()) {
      if (ctrl_blk_ptr->segments.tail->tailroom() == 0 and not append_segment(0, seg->length())) {
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
  if (*other.ctrl_blk_ptr->segment_pool != *ctrl_blk_ptr->segment_pool) {
    // The pools are different. Deep copy segments.
    return append(other);
  }

  // This is the last reference to "after". Shallow copy, except control segment.
  node_t* node = create_segment(0, other.ctrl_blk_ptr->segment_in_cb_memory_block->length());
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

bool byte_buffer::append(const byte_buffer_view& view)
{
  // Append segment by segment.
  auto view_segs = view.segments();
  for (span<const uint8_t> seg : view_segs) {
    if (not append(seg)) {
      return false;
    }
  }
  return true;
}

bool byte_buffer::default_construct_unsafe(byte_buffer_memory_resource& segment_pool, unsigned sz_hint)
{
  if (&segment_pool == &get_default_byte_buffer_segment_pool()) {
    // No need to create a control block yet, if the pool is default one.
    return true;
  }
  // create head segment and assign segment pool to the control block.
  node_t* n = add_head_segment(DEFAULT_FIRST_SEGMENT_HEADROOM, segment_pool, sz_hint);
  return n != nullptr;
}

bool byte_buffer::append(span<const uint8_t> bytes, byte_buffer_memory_resource& segment_pool)
{
  if (bytes.empty()) {
    // No bytes are being appended. However, we may still need to create a control block to store the pool.
    if (not has_ctrl_block()) {
      return default_construct_unsafe(segment_pool, default_segment_size - sizeof(control_block) - sizeof(node_t));
    }
    return true;
  }

  if (not has_ctrl_block()) {
    // Create control block.
    node_t* n =
        add_head_segment(DEFAULT_FIRST_SEGMENT_HEADROOM, segment_pool, bytes.size() + DEFAULT_FIRST_SEGMENT_HEADROOM);
    if (n == nullptr) {
      return false;
    }
  }

  // segment-wise copy.
  for (size_t offset = 0; offset < bytes.size();) {
    if (ctrl_blk_ptr->segments.tail->tailroom() == 0 and not append_segment(0, bytes.size() - offset)) {
      return false;
    }
    size_t              to_write = std::min(ctrl_blk_ptr->segments.tail->tailroom(), bytes.size() - offset);
    span<const uint8_t> subspan  = bytes.subspan(offset, to_write);
    ctrl_blk_ptr->segments.tail->append(subspan);
    offset += to_write;
    ctrl_blk_ptr->pkt_len += to_write;
  }
  return true;
}

byte_buffer::node_t*
byte_buffer::add_head_segment(size_t headroom, byte_buffer_memory_resource& segment_pool, size_t sz_hint)
{
  // Allocate new node.
  span<uint8_t> mem_block = segment_pool.allocate(sz_hint + sizeof(control_block) + sizeof(node_t));
  if (mem_block.empty()) {
    return nullptr;
  }

  // Construct linear allocator pointing to allocated segment memory block.
  linear_memory_allocator arena{mem_block.data(), mem_block.size()};

  // Create control block using allocator.
  void* cb_region = arena.allocate(sizeof(control_block), alignof(control_block));
  ctrl_blk_ptr    = new (cb_region) control_block{};
  srsran_sanity_check(ctrl_blk_ptr != nullptr, "Something went wrong with the creation of the control block");
  ctrl_blk_ptr->segment_pool = &segment_pool;

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

byte_buffer::node_t* byte_buffer::create_segment(size_t headroom, size_t sz_hint)
{
  // Allocate memory block.
  span<uint8_t> mem_block = ctrl_blk_ptr->segment_pool->allocate(sz_hint + sizeof(node_t));
  if (mem_block.empty()) {
    return nullptr;
  }

  // Create a linear allocator pointing to the allocated memory block.
  linear_memory_allocator arena{mem_block.data(), mem_block.size()};

  void*  segment_start = arena.allocate(sizeof(node_t), alignof(node_t));
  size_t segment_size  = arena.nof_bytes_left();
  void*  payload_start = arena.allocate(segment_size, 1);
  return new (segment_start)
      node_t(span<uint8_t>{static_cast<uint8_t*>(payload_start), segment_size}, std::min(headroom, segment_size));
}

bool byte_buffer::append_segment(size_t headroom_suggestion, size_t sz_hint)
{
  if (not has_ctrl_block()) {
    return add_head_segment(headroom_suggestion, get_default_byte_buffer_segment_pool(), sz_hint) != nullptr;
  }
  node_t* segment = create_segment(headroom_suggestion, sz_hint);
  if (segment == nullptr) {
    return false;
  }
  // Append new segment to linked list.
  ctrl_blk_ptr->segments.push_back(*segment);
  return true;
}

bool byte_buffer::prepend_segment(size_t headroom_suggestion, size_t sz_hint)
{
  if (not has_ctrl_block()) {
    return add_head_segment(headroom_suggestion, get_default_byte_buffer_segment_pool(), sz_hint) != nullptr;
  }
  node_t* segment = create_segment(headroom_suggestion, sz_hint);
  if (segment == nullptr) {
    return false;
  }
  // Prepend new segment to linked list.
  ctrl_blk_ptr->segments.push_front(*segment);
  return true;
}

void byte_buffer::pop_last_segment()
{
  node_t* tail = ctrl_blk_ptr->segments.tail;
  if (tail == nullptr) {
    return;
  }

  // Decrement bytes stored in the tail.
  ctrl_blk_ptr->pkt_len -= tail->length();

  // Remove tail from linked list.
  ctrl_blk_ptr->segments.pop_back();

  // Deallocate tail segment.
  ctrl_blk_ptr->destroy_node(tail);
}

bool byte_buffer::prepend(span<const uint8_t> bytes)
{
  if (empty()) {
    // the byte buffer is empty. Prepending is the same as appending.
    return append(bytes);
  }
  for (size_t count = 0; count < bytes.size();) {
    if (ctrl_blk_ptr->segments.head->headroom() == 0) {
      if (not prepend_segment(bytes.size() - count, bytes.size() - count)) {
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
    node_t* node = create_segment(0, seg.size());
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
    // "other" is not the last reference to the underlying byte buffer control block. Deep copy of segments.
    return prepend(other);
  }
  if (*other.ctrl_blk_ptr->segment_pool != *ctrl_blk_ptr->segment_pool) {
    // The pools are different. Deep copy segments.
    return prepend(other);
  }

  // This is the last reference to "other". Shallow copy, except control segment.
  node_t* node = create_segment(0, other.ctrl_blk_ptr->segment_in_cb_memory_block->length());
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
      if (not prepend_segment(rem_bytes, rem_bytes)) {
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
        if (not append_segment(0, to_add)) {
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

expected<byte_buffer> srsran::make_byte_buffer(const std::string& hex_str)
{
  if (hex_str.size() % 2 != 0) {
    // Failed to parse hex string.
    return make_unexpected(default_error_t{});
  }

  byte_buffer ret{byte_buffer::fallback_allocation_tag{}};
  for (size_t i = 0, e = hex_str.size(); i != e; i += 2) {
    uint8_t val;
    if (std::sscanf(hex_str.data() + i, "%02hhX", &val) <= 0) {
      // Failed to parse Hex digit.
      return make_unexpected(default_error_t{});
    }
    bool success = ret.append(val);
    if (not success) {
      // Note: This shouldn't generally happen as we use a fallback allocator.
      return make_unexpected(default_error_t{});
    }
  }
  return ret;
}

span<const uint8_t> srsran::to_span(const byte_buffer& src, span<uint8_t> tmp_mem)
{
  // Empty buffer.
  if (src.empty()) {
    return {};
  }

  // Is contiguous: shortcut without copy.
  if (src.is_contiguous()) {
    return *src.segments().begin();
  }

  // Non-contiguous: copy required.
  srsran_assert(src.length() <= tmp_mem.size(),
                "Insufficient temporary memory to fit the byte_buffer. buffer_size={}, tmp_size={}",
                src.length(),
                tmp_mem.size());
  span<uint8_t> result = {tmp_mem.data(), src.length()};
  copy_segments(src, result);
  return result;
}

// ---- byte_buffer_writer

bool byte_buffer_writer::append_zeros(size_t nof_zeros)
{
  // TODO: optimize.
  for (size_t i = 0; i != nof_zeros; ++i) {
    if (not buffer->append(0)) {
      return false;
    }
  }
  return true;
}
