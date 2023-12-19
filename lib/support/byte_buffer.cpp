/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/detail/byte_buffer_segment_pool.h"

using namespace srsran;

size_t srsran::byte_buffer_segment_pool_default_segment_size()
{
  return 2048;
}

/// Get default byte buffer segment pool. Initialize pool if not initialized before.
detail::byte_buffer_segment_pool& srsran::detail::get_default_byte_buffer_segment_pool()
{
  // Initialize byte buffer segment pool, if not yet initialized.
  // Note: In case of unit tests, this function will be called rather than init_byte_buffer_segment_pool(...)
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

namespace {

/// \brief Linear allocator for memory_block obtained from byte_buffer_segment_pool.
struct memory_arena_linear_allocator {
  /// Pointer to the memory block obtained from byte_buffer_segment_pool.
  void* mem_block = nullptr;
  /// Offset in bytes from the beginning of the memory block, determining where the next allocation will be made.
  size_t offset = 0;

  memory_arena_linear_allocator() noexcept :
    mem_block([]() {
      static auto& pool = detail::get_default_byte_buffer_segment_pool();
      return pool.allocate_node(pool.memory_block_size());
    }())
  {
  }

  void* allocate(size_t sz, size_t al) noexcept
  {
    void* p = align_next(static_cast<char*>(mem_block) + offset, al);
    offset  = (static_cast<char*>(p) - static_cast<char*>(mem_block)) + sz;
    return p;
  }

  bool empty() const { return mem_block == nullptr; }

  size_t space_left() const { return detail::get_default_byte_buffer_segment_pool().memory_block_size() - offset; }
};

/// Allocator for byte_buffer control_block that will leverage the \c memory_arena_linear_allocator.
template <typename T>
struct control_block_allocator {
public:
  using value_type = T;

  template <typename U>
  struct rebind {
    typedef control_block_allocator<U> other;
  };

  control_block_allocator(memory_arena_linear_allocator& arena_) noexcept : arena(&arena_) {}

  control_block_allocator(const control_block_allocator<T>& other) noexcept = default;

  template <typename U, std::enable_if_t<not std::is_same<U, T>::value, int> = 0>
  control_block_allocator(const control_block_allocator<U>& other) noexcept : arena(other.arena)
  {
  }

  control_block_allocator& operator=(const control_block_allocator<T>& other) noexcept = default;

  value_type* allocate(size_t n) noexcept
  {
    srsran_sanity_check(n == 1, "control_block_allocator can only allocate one control block at a time.");
    srsran_sanity_check(not arena->empty(), "Memory arena is empty");
    srsran_assert(arena->space_left() >= sizeof(value_type), "control_block_allocator memory block size is too small.");

    return static_cast<value_type*>(arena->allocate(sizeof(value_type), alignof(std::max_align_t)));
  }

  void deallocate(value_type* p, size_t n) noexcept
  {
    // Note: at this stage the arena ptr is probably dangling. Do not touch it.

    static auto& pool = detail::get_default_byte_buffer_segment_pool();

    srsran_assert(n == 1, "control_block_allocator can only deallocate one control block at a time.");

    pool.deallocate_node(static_cast<void*>(p));
  }

  bool operator==(const control_block_allocator& other) const { return arena == other.arena; }
  bool operator!=(const control_block_allocator& other) const { return !(*this == other); }

private:
  template <typename U>
  friend struct control_block_allocator;

  memory_arena_linear_allocator* arena;
};

} // namespace

void byte_buffer::control_block::destroy_node(node_t* node) const
{
  node->~node_t();
  if (node != segment_in_cb_memory_block) {
    detail::byte_buffer_segment_pool::get_instance().deallocate_node(node);
  }
}

byte_buffer::control_block::~control_block()
{
  // Destroy and return all segments back to the segment memory pool.
  for (node_t* node = segments.head; node != nullptr; node = node->next) {
    destroy_node(node);
  }
}

byte_buffer::node_t* byte_buffer::create_head_segment(size_t headroom)
{
  static auto&        pool       = detail::get_default_byte_buffer_segment_pool();
  static const size_t block_size = pool.memory_block_size();

  // Create control block using allocator.
  memory_arena_linear_allocator arena;
  if (arena.empty()) {
    byte_buffer::warn_alloc_failure();
    return nullptr;
  }
  ctrl_blk_ptr = std::allocate_shared<control_block>(control_block_allocator<control_block>{arena});
  if (ctrl_blk_ptr == nullptr) {
    return nullptr;
  }

  // For first segment of byte_buffer, add a headroom.
  void* segment_start = arena.allocate(sizeof(node_t), alignof(node_t));
  srsran_assert(block_size > arena.offset, "The memory block provided by the pool is too small");
  size_t  segment_size  = block_size - arena.offset;
  void*   payload_start = arena.allocate(segment_size, 1);
  node_t* node          = new (segment_start)
      node_t(span<uint8_t>{static_cast<uint8_t*>(payload_start), segment_size}, std::min(headroom, segment_size));

  // Register segment as sharing the same memory block with control block.
  ctrl_blk_ptr->segment_in_cb_memory_block = node;

  return node;
}

byte_buffer::node_t* byte_buffer::create_segment(size_t headroom)
{
  static auto&        pool       = detail::get_default_byte_buffer_segment_pool();
  static const size_t block_size = pool.memory_block_size();

  // Allocate memory block.
  memory_arena_linear_allocator arena;
  if (arena.empty()) {
    byte_buffer::warn_alloc_failure();
    return nullptr;
  }
  void* segment_start = arena.allocate(sizeof(node_t), alignof(node_t));
  srsran_assert(block_size > arena.offset, "The memory block provided by the pool is too small");
  size_t segment_size  = block_size - arena.offset;
  void*  payload_start = arena.allocate(segment_size, 1);
  return new (segment_start)
      node_t(span<uint8_t>{static_cast<uint8_t*>(payload_start), segment_size}, std::min(headroom, segment_size));
}

bool byte_buffer::append_segment(size_t headroom_suggestion)
{
  node_t* segment = empty() ? create_head_segment(headroom_suggestion) : create_segment(headroom_suggestion);
  if (segment == nullptr) {
    return false;
  }

  // Append new segment to linked list.
  ctrl_blk_ptr->segments.push_back(*segment);
  return true;
}

bool byte_buffer::prepend_segment(size_t headroom_suggestion)
{
  // Note: Add HEADROOM for first segment.
  node_t* segment = empty() ? create_head_segment(headroom_suggestion) : create_segment(headroom_suggestion);
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
    append(std::move(other));
    return true;
  }
  if (not other.ctrl_blk_ptr.unique()) {
    // Deep copy of segments.
    prepend(other);
    return true;
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

void byte_buffer::warn_alloc_failure()
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
  logger.warning("POOL: Failure to allocate byte buffer segment");
}
