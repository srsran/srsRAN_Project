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

#include "ring_memory_allocator.h"
#include "srsran/support/compiler.h"
#include "srsran/support/error_handling.h"

namespace srsran {

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
public:
  using tick_t = unsigned;

private:
  struct overflow_checker {
    std::vector<unsigned> n_alloc_per_tick;
    size_t                pool_size;
    size_t                max_bytes_per_tick;
    tick_t                tic_wraparound;

    bool     first_tick      = true;
    size_t   n_alloc         = 0;
    unsigned ring_index      = 0;
    tick_t   last_tick_index = 0;

    explicit overflow_checker(size_t nof_bytes_per_tick, size_t ring_of_slots_size, tick_t tic_wraparound_) :
      n_alloc_per_tick(ring_of_slots_size, 0),
      pool_size(nof_bytes_per_tick * ring_of_slots_size),
      max_bytes_per_tick(nof_bytes_per_tick),
      tic_wraparound(tic_wraparound_)
    {
    }

    /// \brief Register an allocation of size \sz bytes, in the overflow detector.
    void register_alloc(size_t sz) noexcept
    {
      if (n_alloc + sz > pool_size) {
        report_fatal_error("POOL: Slot ring buffer overflown");
      }
      if (sz > max_bytes_per_tick) {
        report_fatal_error(
            "POOL: Trying to allocate too many bytes in one single tick ({} > {})", sz, max_bytes_per_tick);
      }
      n_alloc_per_tick[ring_index] += sz;
      n_alloc += sz;
    }

    void tick(tick_t tic_index) noexcept
    {
      srsran_assert(first_tick or tic_index != last_tick_index,
                    "This function cannot be called multiple times for the same tick");
      if (SRSRAN_UNLIKELY(first_tick)) {
        // first tick call.
        last_tick_index = tic_index - 1;
        first_tick      = false;
      } else if (SRSRAN_UNLIKELY(last_tick_index > tic_index)) {
        // wrap-around case.
        last_tick_index = -(tic_wraparound - last_tick_index);
      }
      for (; last_tick_index != tic_index; ++last_tick_index) {
        ring_index = (ring_index + 1) % n_alloc_per_tick.size();
        n_alloc -= n_alloc_per_tick[ring_index];
        n_alloc_per_tick[ring_index] = 0;
      }
    }
  };

  struct no_overflow_checker {
    explicit no_overflow_checker(size_t nof_bytes_per_tick, size_t ring_of_slots_size, tick_t /**/) {}

    void register_alloc(size_t sz) noexcept {}
    void tick(unsigned tic_index) noexcept {}
  };

public:
  explicit ticking_ring_buffer_pool(size_t nof_bytes_per_tick,
                                    size_t buffer_duration_in_ticks,
                                    tick_t tic_wraparound_) :
    pool(nof_bytes_per_tick * buffer_duration_in_ticks),
    overflow_detector(nof_bytes_per_tick, buffer_duration_in_ticks, tic_wraparound_)
  {
  }

  /// \brief Allocate span of contiguous bytes of size \c sz. Overflows may be detected, depending on the overflow
  /// detection policy defined.
  /// \return span of bytes pointing at allocated memory region.
  span<uint8_t> allocate_buffer(size_t sz) noexcept
  {
    overflow_detector.register_alloc(sz);
    return pool.allocate_buffer(sz);
  }

  /// \brief Increments tick counter and clears old buffers.
  void tick(tick_t tic_index) noexcept { overflow_detector.tick(tic_index); }

  /// \brief Size of the ring buffer pool.
  size_t size() const { return pool.size(); }

private:
  using overflow_detector_t =
      std::conditional_t<SRSRAN_IS_DEFINED(ASSERTS_ENABLED), overflow_checker, no_overflow_checker>;

  ring_buffer_pool    pool;
  overflow_detector_t overflow_detector;
};

} // namespace srsran
