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

#include "srsran/support/memory_pool/ring_buffer_pool.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(ring_buffer_pool,
     when_pool_is_called_multiple_consecutive_times_then_the_allocated_byte_spans_are_contiguous_in_memory)
{
  ring_buffer_pool pool(128);

  span<const uint8_t> total_allocs;
  for (unsigned i = 0; i < 128;) {
    unsigned      sz     = test_rgen::uniform_int<unsigned>(1, 128 - i);
    span<uint8_t> buffer = pool.allocate_buffer(sz);
    ASSERT_EQ(buffer.size(), sz) << "Invalid byte span size returned from the allocator";
    if (total_allocs.empty()) {
      // first time.
      total_allocs = buffer;
    } else {
      ASSERT_EQ(total_allocs.end(), buffer.begin())
          << "The returned byte span should be contiguous with previous allocation";
      total_allocs = {total_allocs.data(), buffer.data() + buffer.size()};
    }
    i += sz;
  }
}

TEST(ring_buffer_pool, when_pool_limit_is_reached_then_next_allocated_byte_span_wraps_around_the_pool)
{
  ring_buffer_pool pool(test_rgen::uniform_int<unsigned>(2, 256));

  unsigned      sz            = test_rgen::uniform_int<unsigned>(1, pool.size());
  span<uint8_t> first_buffer  = pool.allocate_buffer(sz);
  unsigned      sz2           = std::min(pool.size() - sz + 1, pool.size());
  span<uint8_t> second_buffer = pool.allocate_buffer(sz2);
  ASSERT_EQ(first_buffer.size(), sz);
  ASSERT_EQ(second_buffer.size(), sz2);
  ASSERT_EQ(first_buffer.begin(), second_buffer.begin());
}

TEST(ticking_ring_buffer_pool, ticking_clears_allocated_bytes)
{
  unsigned                 nof_ticks_depth    = test_rgen::uniform_int<unsigned>(32, 256);
  unsigned                 max_bytes_per_tick = test_rgen::uniform_int<unsigned>(32, 256);
  ticking_ring_buffer_pool pool(
      max_bytes_per_tick, nof_ticks_depth, std::numeric_limits<ticking_ring_buffer_pool::tick_t>::max());

  // Deplete pool.
  unsigned tic = test_rgen::uniform_int<ticking_ring_buffer_pool::tick_t>();
  for (unsigned i = 0; i != nof_ticks_depth; ++i) {
    pool.tick(tic++);

    pool.allocate_buffer(max_bytes_per_tick);
  }

  for (unsigned i = 0; i != nof_ticks_depth; ++i) {
    // Deallocates memory allocated in a given tick.
    pool.tick(tic++);

    // Pool should not overflow.
    pool.allocate_buffer(max_bytes_per_tick);
  }
}

#if ASSERTS_ENABLED
TEST(ticking_ring_buffer_pool, tick_wrap_around_does_not_erroneously_deallocate_buffers)
{
  unsigned                 nof_ticks_depth    = test_rgen::uniform_int<unsigned>(32, 256);
  unsigned                 max_bytes_per_tick = test_rgen::uniform_int<unsigned>(32, 256);
  unsigned                 wrap_around        = 10240;
  ticking_ring_buffer_pool pool(max_bytes_per_tick, nof_ticks_depth, wrap_around);

  // Deplete pool and wrap-around in the process.
  ticking_ring_buffer_pool::tick_t tick =
      wrap_around - test_rgen::uniform_int<ticking_ring_buffer_pool::tick_t>(1, nof_ticks_depth - 1);
  for (unsigned i = 0; i != nof_ticks_depth; ++i) {
    pool.tick(tick);
    tick = (tick + 1) % wrap_around;
    pool.allocate_buffer(max_bytes_per_tick);
  }
  ASSERT_DEATH(pool.allocate_buffer(max_bytes_per_tick), "overflow");
}
#endif
