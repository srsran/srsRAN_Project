/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/triple_buffer_atomic.h"
#include <gtest/gtest.h>
#include <thread>

using namespace srsran;

TEST(triple_buffer_atomic, basic)
{
  triple_buffer_atomic<int> trio{};

  constexpr unsigned N               = 100;
  auto               producer_thread = std::thread([&]() {
    for (unsigned i = 1; i <= N; ++i) {
      trio.write_and_commit(i);
    }
  });

  int prev_read = 0;
  while (prev_read != N) {
    const auto& val = trio.read();
    ASSERT_GE(val, prev_read);
    prev_read = val;
  }
  producer_thread.join();

  const auto& val = trio.read();
  ASSERT_EQ(val, N);
}
