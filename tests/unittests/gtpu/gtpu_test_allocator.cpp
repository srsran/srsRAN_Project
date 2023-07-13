/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

//// GTPU alloc test
TEST(gtpu_pool_test, normal_request_succeeds)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = 0; n < 3; ++n) {
    gtpu_teid_t teid;
    ASSERT_EQ(true, pool.request_teid(teid));
    ASSERT_EQ(teid, gtpu_teid_t{n});
  }
}

TEST(gtpu_pool_test, full_pool_request_fails)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = 0; n < MAX_TEIDS; ++n) {
    gtpu_teid_t teid;
    ASSERT_EQ(true, pool.request_teid(teid));
    ASSERT_EQ(teid, gtpu_teid_t{n});
  }
  gtpu_teid_t teid;
  ASSERT_EQ(false, pool.request_teid(teid));
}

TEST(gtpu_pool_test, request_after_all_release_succeeds)
{
  gtpu_teid_pool_impl alloc(MAX_TEIDS);

  for (unsigned n = 0; n < MAX_TEIDS; ++n) {
    gtpu_teid_t teid;
    ASSERT_EQ(true, alloc.request_teid(teid));
    ASSERT_EQ(teid, gtpu_teid_t{n});
    ASSERT_EQ(true, alloc.release_teid(teid));
  }
  gtpu_teid_t teid;
  ASSERT_EQ(true, alloc.request_teid(teid));
}

TEST(gtpu_pool_test, request_after_few_free_succeeds)
{
  gtpu_teid_pool_impl pool(MAX_TEIDS);

  for (unsigned n = 0; n < MAX_TEIDS; ++n) {
    gtpu_teid_t teid;
    ASSERT_EQ(true, pool.request_teid(teid));
    ASSERT_EQ(teid, gtpu_teid_t{n});
  }

  // free the TEID 6 and 8
  {
    ASSERT_EQ(true, pool.release_teid(gtpu_teid_t{6}));
    ASSERT_EQ(true, pool.release_teid(gtpu_teid_t{8}));
  }
  {
    gtpu_teid_t teid;
    ASSERT_EQ(true, pool.request_teid(teid));
    ASSERT_EQ(teid, gtpu_teid_t{6});
  }
  {
    gtpu_teid_t teid;
    ASSERT_EQ(true, pool.request_teid(teid));
    ASSERT_EQ(teid, gtpu_teid_t{8});
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
