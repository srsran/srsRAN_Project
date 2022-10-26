/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/memory_pool/ring_buffer_pool.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

TEST(ring_buffer_pool, contiguous_buffers_are_allocated)
{
  ring_buffer_pool pool(128);

  span<const uint8_t> total_allocs;
  for (unsigned i = 0; i < 128;) {
    unsigned      sz     = get_random_uint(1, 128 - i);
    span<uint8_t> buffer = pool.allocate_buffer(sz);
    ASSERT_EQ(buffer.size(), sz);
    if (total_allocs.empty()) {
      // first time.
      total_allocs = buffer;
    } else {
      ASSERT_EQ(total_allocs.end(), buffer.begin());
      total_allocs = {total_allocs.data(), buffer.data() + buffer.size()};
    }
    i += sz;
  }
}

TEST(ring_buffer_pool, buffers_wrap_around_on_ring_limit)
{
  ring_buffer_pool pool(get_random_uint(2, 256));

  unsigned      sz            = get_random_uint(1, pool.size());
  span<uint8_t> first_buffer  = pool.allocate_buffer(sz);
  unsigned      sz2           = std::min(pool.size() - sz + 1, pool.size());
  span<uint8_t> second_buffer = pool.allocate_buffer(sz2);
  ASSERT_EQ(first_buffer.size(), sz);
  ASSERT_EQ(second_buffer.size(), sz2);
  ASSERT_EQ(first_buffer.begin(), second_buffer.begin());
}

TEST(ticking_ring_buffer_pool, ticking_clears_allocated_bytes)
{
  unsigned                 nof_ticks_depth    = get_random_uint(32, 256);
  unsigned                 max_bytes_per_tick = get_random_uint(32, 256);
  ticking_ring_buffer_pool pool(max_bytes_per_tick, nof_ticks_depth);

  // Deplete pool.
  unsigned tic = 0;
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
