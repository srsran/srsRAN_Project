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

#include "lib/rlc/rlc_retx_queue.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture class for RLC ReTx queue tests
class rlc_retx_queue_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);

    logger.info("Creating RLC ReTx queue");

    // Create RLC ReTx queue
    rq = std::make_unique<rlc_retx_queue>(retx_queue_size);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&           logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<rlc_retx_queue> rq;

  const size_t retx_queue_size = 4;
};

TEST_F(rlc_retx_queue_test, create_new_entity)
{
  EXPECT_TRUE(rq->empty());
  EXPECT_EQ(rq->size(), 0);
  const rlc_retx_queue_state state = rq->state();
  EXPECT_EQ(state.get_retx_bytes(), 0);
  EXPECT_EQ(state.get_n_retx_so_zero(), 0);
  EXPECT_EQ(state.get_n_retx_so_nonzero(), 0);
}

TEST_F(rlc_retx_queue_test, push_replace_front)
{
  rlc_tx_amd_retx retx;
  retx.sn     = 77;
  retx.so     = 6;
  retx.length = 555;
  EXPECT_TRUE(rq->try_push(retx));
  EXPECT_TRUE(rq->try_push(retx));
  EXPECT_FALSE(rq->empty());
  EXPECT_EQ(rq->size(), 2);

  EXPECT_EQ(rq->state().get_n_retx_so_zero(), 0);
  EXPECT_EQ(rq->state().get_n_retx_so_nonzero(), 2);
  EXPECT_EQ(rq->state().get_retx_bytes(), 2 * retx.length);

  rlc_tx_amd_retx new_retx;
  new_retx.sn     = 88;
  new_retx.so     = 0;
  new_retx.length = 6666;

  rq->replace_front(new_retx);
  EXPECT_EQ(rq->size(), 2);

  EXPECT_EQ(rq->front().sn, new_retx.sn);
  EXPECT_EQ(rq->front().so, new_retx.so);
  EXPECT_EQ(rq->front().length, new_retx.length);

  EXPECT_EQ(rq->state().get_n_retx_so_zero(), 1);
  EXPECT_EQ(rq->state().get_n_retx_so_nonzero(), 1);
  EXPECT_EQ(rq->state().get_retx_bytes(), retx.length + new_retx.length);
}

TEST_F(rlc_retx_queue_test, push_clear)
{
  rlc_tx_amd_retx retx;
  retx.sn     = 77;
  retx.so     = 6;
  retx.length = 555;
  EXPECT_TRUE(rq->try_push(retx));
  EXPECT_TRUE(rq->try_push(retx));
  EXPECT_FALSE(rq->empty());
  EXPECT_EQ(rq->size(), 2);
  rq->clear();
  EXPECT_TRUE(rq->empty());
  EXPECT_EQ(rq->size(), 0);
  const rlc_retx_queue_state state = rq->state();
  EXPECT_EQ(state.get_retx_bytes(), 0);
  EXPECT_EQ(state.get_n_retx_so_zero(), 0);
  EXPECT_EQ(state.get_n_retx_so_nonzero(), 0);
}

TEST_F(rlc_retx_queue_test, push_pop)
{
  // create testdata
  std::vector<rlc_tx_amd_retx> retx_data;
  for (uint32_t i = 0; i < retx_queue_size; i++) {
    rlc_tx_amd_retx retx;
    retx.sn     = i + 1;
    retx.so     = i + 0; // the first ReTx has zero SO and counts to "retx_so_zero"; others are "retx_so_nonzero"
    retx.length = i + 2;
    retx_data.push_back(std::move(retx));
  }

  uint32_t nof_refills = 2;

  for (uint32_t n = 0; n < nof_refills; n++) {
    uint32_t queued_bytes = 0;
    for (uint32_t i = 0; i < retx_data.size(); i++) {
      // push ReTx
      queued_bytes += retx_data[i].length;
      EXPECT_TRUE(rq->try_push(retx_data[i]));
      EXPECT_FALSE(rq->empty());
      EXPECT_EQ(rq->size(), i + 1);
      // validate queue state
      const rlc_retx_queue_state state = rq->state();
      EXPECT_EQ(state.get_retx_bytes(), queued_bytes);
      EXPECT_EQ(state.get_n_retx_so_zero(), 1);
      EXPECT_EQ(state.get_n_retx_so_nonzero(), i);
    }

    for (uint32_t i = 0; i < retx_data.size(); i++) {
      ASSERT_FALSE(rq->empty());
      EXPECT_EQ(rq->size(), retx_data.size() - i);
      // validate front ReTx
      const rlc_tx_amd_retx& retx = rq->front();
      EXPECT_EQ(retx.sn, retx_data[i].sn);
      EXPECT_EQ(retx.so, retx_data[i].so);
      EXPECT_EQ(retx.length, retx_data[i].length);
      queued_bytes -= retx.length;
      rq->pop();
      // validate queue state
      const rlc_retx_queue_state state = rq->state();
      EXPECT_EQ(state.get_retx_bytes(), queued_bytes);
      EXPECT_EQ(state.get_n_retx_so_zero(), 0);
      EXPECT_EQ(state.get_n_retx_so_nonzero(), retx_data.size() - i - 1);
    }
    EXPECT_TRUE(rq->empty());
    EXPECT_EQ(rq->size(), 0);
  }
}

TEST_F(rlc_retx_queue_test, try_push_when_no_space_left)
{
  // create testdata
  std::vector<rlc_tx_amd_retx> retx_data;
  for (uint32_t i = 0; i < retx_queue_size; i++) {
    rlc_tx_amd_retx retx;
    retx.sn     = i + 1;
    retx.so     = i + 0; // the first ReTx has zero SO and counts to "retx_so_zero"; others are "retx_so_nonzero"
    retx.length = i + 2;
    retx_data.push_back(std::move(retx));
  }

  uint32_t queued_bytes = 0;
  for (uint32_t i = 0; i < retx_data.size(); i++) {
    // push ReTx
    queued_bytes += retx_data[i].length;
    EXPECT_TRUE(rq->try_push(retx_data[i]));
  }
  // queue is full
  EXPECT_EQ(rq->size(), retx_data.size());

  rlc_tx_amd_retx another_retx;
  another_retx.sn     = 77;
  another_retx.so     = 6;
  another_retx.length = 555;

  // try to push another ReTx while removing all elements except for the first to block the queue
  for (uint32_t i = 1; i < retx_data.size(); i++) {
    EXPECT_FALSE(rq->try_push(another_retx));

    // queue is still full (valid+invalid)
    EXPECT_EQ(rq->size(), retx_data.size());

    // validate no side-effects on front ReTx
    const rlc_tx_amd_retx& retx = rq->front();
    EXPECT_EQ(retx.sn, retx_data[0].sn);
    EXPECT_EQ(retx.so, retx_data[0].so);
    EXPECT_EQ(retx.length, retx_data[0].length);

    // validate no side_effects on queue state
    rlc_retx_queue_state state = rq->state();
    EXPECT_EQ(state.get_retx_bytes(), queued_bytes);
    EXPECT_EQ(state.get_n_retx_so_zero(), 1);
    EXPECT_EQ(state.get_n_retx_so_nonzero(), retx_data.size() - i);

    // remove i-th element with SN=i+1
    EXPECT_TRUE(rq->remove_sn(i + 1));
    queued_bytes -= retx_data[i].length;
  }

  EXPECT_FALSE(rq->empty());
  EXPECT_EQ(rq->size(), retx_data.size());

  // remove front element that blocks the cleanup of removed/invalid elements
  rq->pop();
  EXPECT_TRUE(rq->empty());
  EXPECT_EQ(rq->size(), 0);

  // validate queue state
  rlc_retx_queue_state state = rq->state();
  EXPECT_EQ(state.get_retx_bytes(), 0);
  EXPECT_EQ(state.get_n_retx_so_zero(), 0);
  EXPECT_EQ(state.get_n_retx_so_nonzero(), 0);

  // now we can push again
  EXPECT_TRUE(rq->try_push(another_retx));
  EXPECT_FALSE(rq->empty());

  // validate front
  EXPECT_EQ(rq->front().sn, another_retx.sn);
  EXPECT_EQ(rq->front().so, another_retx.so);
  EXPECT_EQ(rq->front().length, another_retx.length);

  // validate state
  state = rq->state();
  EXPECT_EQ(state.get_retx_bytes(), another_retx.length);
  EXPECT_EQ(state.get_n_retx_so_zero(), 0);
  EXPECT_EQ(state.get_n_retx_so_nonzero(), 1);
}

TEST_F(rlc_retx_queue_test, push_has_sn_remove_sn)
{
  // SN=88 is not present (empty)
  EXPECT_FALSE(rq->has_sn(88));

  rlc_tx_amd_retx retx0;
  retx0.sn     = 77;
  retx0.so     = 6;
  retx0.length = 555;
  EXPECT_TRUE(rq->try_push(retx0));

  // SN=88 is still not present (not found)
  EXPECT_FALSE(rq->has_sn(88));

  rlc_tx_amd_retx retx1;
  retx1.sn     = 88;
  retx1.so     = 7;
  retx1.length = 6666;
  EXPECT_TRUE(rq->try_push(retx1));

  // SN=88 is present
  EXPECT_TRUE(rq->has_sn(88));

  rlc_tx_amd_retx retx2;
  retx2.sn     = 88;
  retx2.so     = 7777;
  retx2.length = 8888;
  EXPECT_TRUE(rq->try_push(retx2));

  // SN=88 is present (2 times)
  EXPECT_TRUE(rq->has_sn(88));

  // Check SN with matching SO+length
  EXPECT_TRUE(rq->has_sn(retx2.sn, retx2.so, retx2.length));
  // Check SN with matching length but SO offset
  EXPECT_TRUE(rq->has_sn(retx2.sn, retx2.so + 1, retx2.length - 1));
  // Check SN with matching SO but shorter length
  EXPECT_TRUE(rq->has_sn(retx2.sn, retx2.so, retx2.length - 1));
  // Check SN with matching length but shorter SO offset
  EXPECT_FALSE(rq->has_sn(retx2.sn, retx2.so - 1, retx2.length + 1));
  // Check SN with matching SO but larger length
  EXPECT_FALSE(rq->has_sn(retx2.sn, retx2.so, retx2.length + 1));

  // Remove non-existing SN
  EXPECT_FALSE(rq->remove_sn(99));
  EXPECT_EQ(rq->state().get_n_retx_so_nonzero(), 3);

  // Remove SN=88 (two elements)
  EXPECT_TRUE(rq->remove_sn(88));
  EXPECT_EQ(rq->state().get_n_retx_so_nonzero(), 1);
  EXPECT_FALSE(rq->has_sn(88));

  // Remove SN=88 which doesn't exist anymore (but the invalid elements are still there)
  EXPECT_FALSE(rq->remove_sn(88));
  EXPECT_EQ(rq->state().get_n_retx_so_nonzero(), 1);
  EXPECT_EQ(rq->size(), 3); // valid + invalid elements

  // SN=77 is still present
  EXPECT_TRUE(rq->has_sn(77));

  // Remove SN=77
  EXPECT_TRUE(rq->remove_sn(77));
  EXPECT_EQ(rq->state().get_n_retx_so_nonzero(), 0);
  EXPECT_FALSE(rq->has_sn(77));
  EXPECT_EQ(rq->size(), 0); // valid + invalid elements
  EXPECT_TRUE(rq->empty());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
