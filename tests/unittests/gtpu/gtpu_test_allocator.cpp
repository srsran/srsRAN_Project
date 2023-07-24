/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "lib/gtpu/gtpu_teid_pool_impl.h"
#include <gtest/gtest.h>

using namespace srsran;

constexpr uint16_t MAX_TEIDS = 16;

/// GTPU pool test
TEST(gtpu_pool_test, normal_request_succeeds)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = 0; n < 3; ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
  }
}

TEST(gtpu_pool_test, full_pool_request_fails)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = 0; n < MAX_TEIDS; ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
  }
  expected<gtpu_teid_t> teid = pool.request_teid();
  ASSERT_EQ(false, teid.has_value());
}

TEST(gtpu_pool_test, request_after_all_release_succeeds)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = 0; n < MAX_TEIDS; ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
    ASSERT_EQ(true, pool.release_teid(teid.value()));
  }
  expected<gtpu_teid_t> teid = pool.request_teid();
  ASSERT_EQ(true, teid.has_value());
}

TEST(gtpu_pool_test, request_after_few_free_succeeds)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = 0; n < MAX_TEIDS; ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
  }

  // free the TEID 6 and 8
  {
    ASSERT_EQ(true, pool.release_teid(gtpu_teid_t{6}));
    ASSERT_EQ(true, pool.release_teid(gtpu_teid_t{8}));
  }
  {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{6});
  }
  {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{8});
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
