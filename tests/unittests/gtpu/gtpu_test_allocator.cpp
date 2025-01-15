/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  for (unsigned n = GTPU_TEID_MIN.value(); n < 3 + GTPU_TEID_MIN.value(); ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
  }
}

TEST(gtpu_pool_test, full_pool_request_fails)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = GTPU_TEID_MIN.value(); n < MAX_TEIDS + GTPU_TEID_MIN.value(); ++n) {
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

  for (unsigned n = GTPU_TEID_MIN.value(); n < MAX_TEIDS + GTPU_TEID_MIN.value(); ++n) {
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

  for (unsigned n = GTPU_TEID_MIN.value(); n < MAX_TEIDS + GTPU_TEID_MIN.value(); ++n) {
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
  // full again
  {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(false, teid.has_value());
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
