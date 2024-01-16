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

#include "lib/sdap/sdap_entity_tx_impl.h"
#include "srsran/sdap/sdap.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;
using namespace srs_cu_up;

/// Mocking class of the surrounding layers invoked by the SDAP TX entity.
class sdap_tx_test_frame : public sdap_tx_pdu_notifier
{
public:
  std::queue<byte_buffer> pdu_queue;

  // sdap_tx_pdu_notifier interface
  void on_new_pdu(byte_buffer pdu) override { pdu_queue.push(std::move(pdu)); };
};

/// Fixture class for SDAP TX tests
class sdap_tx_test : public ::testing::Test
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

    logger.info("Creating SDAP TX entity.");

    // Create test frame
    tester = std::make_unique<sdap_tx_test_frame>();

    // Create SDAP TX entity
    ue_inactivity_timer = timers.create_timer();
    ue_inactivity_timer.set(std::chrono::milliseconds(10000), [](timer_id_t) {});
    sdap = std::make_unique<sdap_entity_tx_impl>(
        7, pdu_session_id_t::min, qos_flow_id_t::min, drb_id_t::drb1, ue_inactivity_timer, *tester);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&                logger = srslog::fetch_basic_logger("TEST", false);
  manual_task_worker                   worker{64};
  timer_manager                        timers_manager;
  timer_factory                        timers{timers_manager, worker};
  unique_timer                         ue_inactivity_timer;
  std::unique_ptr<sdap_tx_test_frame>  tester;
  std::unique_ptr<sdap_entity_tx_impl> sdap;
};

/// \brief Test correct creation of SDAP TX entity
TEST_F(sdap_tx_test, create_new_entity)
{
  ASSERT_NE(sdap, nullptr);
}

/// \brief Test simple PDU transmission
TEST_F(sdap_tx_test, test_tx)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu{sdu_buf};

  sdap->handle_sdu(sdu.deep_copy());

  ASSERT_FALSE(tester->pdu_queue.empty());
  EXPECT_EQ(tester->pdu_queue.front(), sdu);
  tester->pdu_queue.pop();
  ASSERT_TRUE(tester->pdu_queue.empty());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
