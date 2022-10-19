/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ring_memory_allocator.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

/// \brief This pool returns spans of bytes, which have been allocated using a ring memory allocator. It assumes
/// that the underlying memory resource of size \c mem_size_bytes is large enough so that no overflows ever happen.
class ring_buffer_pool
{
public:
  explicit ring_buffer_pool(size_t mem_size_bytes) : mem_res(mem_size_bytes), alloc(mem_res.memory_block()) {}

  /// Allocate \c sz bytes from the ring pool.
  span<uint8_t> allocate_buffer(size_t sz) noexcept
  {
    uint8_t* data_ptr = static_cast<uint8_t*>(alloc.allocate(sz));
    return span<uint8_t>{data_ptr, sz};
  }

  /// Number of bytes held by the memory pool.
  size_t size() const { return mem_res.size(); }

private:
  heap_memory_resource  mem_res;
  ring_memory_allocator alloc;
};

/// \brief This pool returns spans of bytes, which have been allocated using a ring memory allocator. Differently from
/// \c ring_buffer_pool, this pool keeps track of previously allocated bytes, and, on each tick, deallocates the memory
/// that it deems to be too old. This extra check gives this pool the ability to detect potential buffer overflows.
class ticking_ring_buffer_pool
{
  struct overflow_checker {
    constexpr static size_t invalid_tick_index = std::numeric_limits<unsigned>::max();

    std::vector<unsigned> n_alloc_per_tick;
    size_t                pool_size;
    size_t                max_bytes_per_tick;
    size_t                n_alloc         = 0;
    unsigned              tick_counter    = 0;
    unsigned              last_tick_index = 0;

    explicit overflow_checker(size_t nof_bytes_per_tick, size_t ring_of_slots_size) :
      n_alloc_per_tick(ring_of_slots_size, 0),
      pool_size(nof_bytes_per_tick * ring_of_slots_size),
      max_bytes_per_tick(nof_bytes_per_tick)
    {
    }

    void register_alloc(size_t sz) noexcept
    {
      if (n_alloc + sz > pool_size) {
        report_fatal_error("POOL: Slot ring buffer overflown");
      }
      if (sz > max_bytes_per_tick) {
        report_fatal_error(
            "POOL: Trying to allocate too many bytes in one single tick ({} > {})", sz, max_bytes_per_tick);
      }
      n_alloc_per_tick[tick_counter] += sz;
      n_alloc += sz;
    }

    void tick(unsigned tic_index) noexcept
    {
      if (srsgnb_unlikely(last_tick_index == invalid_tick_index)) {
        // first tick call.
        last_tick_index = tic_index - 1;
      }
      tick_counter = (tick_counter + 1) % n_alloc_per_tick.size();
      for (; last_tick_index != tic_index; ++last_tick_index) {
        n_alloc -= n_alloc_per_tick[tick_counter];
        n_alloc_per_tick[tick_counter] = 0;
      }
    }
  };

  struct no_overflow_checker {
    explicit no_overflow_checker(size_t nof_bytes_per_tick, size_t ring_of_slots_size) {}

    void register_alloc(size_t sz) noexcept {}
    void tick(unsigned tic_index) noexcept {}
  };

public:
  explicit ticking_ring_buffer_pool(size_t nof_bytes_per_tick, size_t buffer_duration_in_ticks) :
    pool(nof_bytes_per_tick * buffer_duration_in_ticks), overflow_detector(nof_bytes_per_tick, buffer_duration_in_ticks)
  {
  }

  /// \brief Allocate span of contiguous bytes of size \c sz. Overflows may be detected, depending on the overflow
  /// detection policy defined.
  span<uint8_t> allocate_buffer(size_t sz) noexcept
  {
    overflow_detector.register_alloc(sz);
    return pool.allocate_buffer(sz);
  }

  /// Increments tick counter and clears old buffers.
  void tick(unsigned tic_index) noexcept { overflow_detector.tick(tic_index); }

  /// Size of the ring buffer pool.
  size_t size() const { return pool.size(); }

private:
  using overflow_detector_t = std::conditional_t<ASSERTS_ENABLED, overflow_checker, no_overflow_checker>;

  ring_buffer_pool    pool;
  overflow_detector_t overflow_detector;
};

} // namespace srsgnb