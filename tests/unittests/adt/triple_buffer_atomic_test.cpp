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
