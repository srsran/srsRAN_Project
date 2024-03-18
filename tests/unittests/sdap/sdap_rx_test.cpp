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

#include "lib/sdap/sdap_entity_rx_impl.h"
#include "srsran/sdap/sdap.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;
using namespace srs_cu_up;

/// Mocking class of the surrounding layers invoked by the SDAP RX entity.
class sdap_rx_test_frame : public sdap_rx_sdu_notifier
{
public:
  std::queue<byte_buffer> sdu_queue;

  // sdap_rx_sdu_notifier interface
  void on_new_sdu(byte_buffer pdu, qos_flow_id_t qfi) override { sdu_queue.push(std::move(pdu)); };
};

/// Fixture class for SDAP RX tests
class sdap_rx_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init SDAP logger
    srslog::fetch_basic_logger("SDAP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SDAP", false).set_hex_dump_max_size(-1);

    logger.info("Creating SDAP RX entity.");

    // Create test frame
    tester = std::make_unique<sdap_rx_test_frame>();

    // Create SDAP RX entity
    sdap = std::make_unique<sdap_entity_rx_impl>(7, pdu_session_id_t::min, qos_flow_id_t::min, drb_id_t::drb1, *tester);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&                logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<sdap_rx_test_frame>  tester;
  std::unique_ptr<sdap_entity_rx_impl> sdap;
};

/// \brief Test correct creation of SDAP RX entity
TEST_F(sdap_rx_test, create_new_entity)
{
  ASSERT_NE(sdap, nullptr);
}

/// \brief Test simple PDU transmission
TEST_F(sdap_rx_test, test_rx)
{
  const std::array<uint8_t, 4> pdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  pdu     = byte_buffer::create(pdu_buf).value();

  sdap->handle_pdu(pdu.deep_copy().value());

  ASSERT_FALSE(tester->sdu_queue.empty());
  EXPECT_EQ(tester->sdu_queue.front(), pdu);
  tester->sdu_queue.pop();
  ASSERT_TRUE(tester->sdu_queue.empty());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
