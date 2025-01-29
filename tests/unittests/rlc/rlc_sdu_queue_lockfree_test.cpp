
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

#include "lib/rlc/rlc_bearer_logger.h"
#include "lib/rlc/rlc_sdu_queue_lockfree.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/du_types.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture class for RLC SDU queue tests
class rlc_sdu_queue_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    test_logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);

    test_logger.info("Creating RLC SDU queue");
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  ///  Default value for RLC SDU queue limit in bytes are chosen such that it allows for 4096 PDCP pdus of 1500 of
  ///  payload and 7 bytes of PDCP overhead. The SDU limit should be much larger then this, so that the limit is the
  ///  number of bytes in the queue, not the number of SDUs, even in the case of small PDUs
  static constexpr uint32_t default_rlc_queue_size_sdus  = 16384;
  static constexpr uint32_t default_rlc_queue_size_bytes = 4096 * (1500 + 7);

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST", false);
  rlc_bearer_logger     rlc_logger  = {"RLC",
                                       {gnb_du_id_t::min, du_ue_index_t::MIN_DU_UE_INDEX, rb_id_t(drb_id_t::drb1), "DL"}};
};

TEST_F(rlc_sdu_queue_test, queue_unqueue)
{
  rlc_sdu_queue_lockfree tx_queue(default_rlc_queue_size_sdus, default_rlc_queue_size_bytes, rlc_logger);

  // Write 1 SDU
  byte_buffer buf       = byte_buffer::create({0x00, 0x01}).value();
  rlc_sdu     write_sdu = {std::move(buf), 10};
  ASSERT_TRUE(tx_queue.write(std::move(write_sdu)));

  // Check basic stats
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(1, state.n_sdus);
  ASSERT_EQ(2, state.n_bytes);

  // Read one SDU
  rlc_sdu read_sdu;
  ASSERT_TRUE(tx_queue.read(read_sdu));

  // Check basic stats
  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);

  // Check SDU
  byte_buffer expected_msg = byte_buffer::create({0x00, 0x01}).value();
  ASSERT_TRUE(read_sdu.pdcp_sn.has_value());
  ASSERT_EQ(10, read_sdu.pdcp_sn.value());
  ASSERT_EQ(expected_msg, read_sdu.buf);
}

TEST_F(rlc_sdu_queue_test, full_capacity)
{
  unsigned capacity = 5;

  rlc_sdu_queue_lockfree tx_queue(capacity, default_rlc_queue_size_bytes, rlc_logger);

  // Write Capacity + 1 SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity + 1; pdcp_sn++) {
    byte_buffer buf = {};
    ASSERT_TRUE(buf.append(pdcp_sn));
    ASSERT_TRUE(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    if (pdcp_sn != capacity) {
      ASSERT_TRUE(tx_queue.write(std::move(write_sdu)) == true);
    } else {
      ASSERT_TRUE(tx_queue.write(std::move(write_sdu)) == false);
    }
  }
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(capacity, state.n_sdus);
  ASSERT_EQ(2 * capacity, state.n_bytes);

  // Read all SDUs and try to read on SDU over capacity
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity + 1; pdcp_sn++) {
    byte_buffer expected_msg = {};
    ASSERT_TRUE(expected_msg.append(pdcp_sn));
    ASSERT_TRUE(expected_msg.append(pdcp_sn));
    rlc_sdu read_sdu = {};
    if (pdcp_sn != capacity) {
      ASSERT_TRUE(tx_queue.read(read_sdu));
      ASSERT_TRUE(expected_msg == read_sdu.buf);
    } else {
      ASSERT_TRUE(false == tx_queue.read(read_sdu));
    }
  }

  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);
}

TEST_F(rlc_sdu_queue_test, discard)
{
  unsigned       capacity          = 10;
  unsigned       n_sdus            = capacity;
  const uint32_t queue_bytes_limit = 6172672;

  rlc_sdu_queue_lockfree tx_queue(capacity, queue_bytes_limit, rlc_logger);

  // Fill SDU queue with SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    byte_buffer buf = {};
    ASSERT_TRUE(buf.append(pdcp_sn));
    ASSERT_TRUE(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    ASSERT_TRUE(tx_queue.write(std::move(write_sdu)));
  }
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(n_sdus, state.n_sdus);
  ASSERT_EQ(2 * n_sdus, state.n_bytes);

  // Discard pdcp_sn 2 and 4
  ASSERT_TRUE(tx_queue.try_discard(2));
  ASSERT_TRUE(tx_queue.try_discard(4));

  // Try to discard non-existing pdcp_sn
  ASSERT_TRUE(false == tx_queue.try_discard(16));

  // Double check correct number of SDUs and SDU bytes
  unsigned leftover_sdus = n_sdus - 2;
  state                  = tx_queue.get_state();
  ASSERT_EQ(leftover_sdus, state.n_sdus);
  ASSERT_EQ(leftover_sdus * 2, state.n_bytes);

  // Read SDUs
  for (uint32_t n = 0; n < leftover_sdus; n++) {
    rlc_sdu read_sdu = {};
    ASSERT_TRUE(tx_queue.read(read_sdu));
  }
  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);
}

TEST_F(rlc_sdu_queue_test, discard_all)
{
  unsigned capacity = 10;
  unsigned n_sdus   = capacity / 2;

  rlc_sdu_queue_lockfree tx_queue(capacity, default_rlc_queue_size_bytes, rlc_logger);

  // Fill SDU queue with SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    byte_buffer buf = {};
    ASSERT_TRUE(buf.append(pdcp_sn));
    ASSERT_TRUE(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    ASSERT_TRUE(tx_queue.write(std::move(write_sdu)) == true);
  }
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(n_sdus, state.n_sdus);
  ASSERT_EQ(2 * n_sdus, state.n_bytes);

  // Discard all SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    ASSERT_TRUE(tx_queue.try_discard(pdcp_sn));
  }

  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);

  // Read SDU
  {
    rlc_sdu read_sdu = {};
    ASSERT_TRUE(tx_queue.read(read_sdu) == false);
  }
  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
