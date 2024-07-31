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

#include "lib/rlc/rlc_rx_tm_entity.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "srsran/rlc/rlc_srb_config_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC TM Rx entity.
class rlc_rx_tm_test_frame : public rlc_rx_upper_layer_data_notifier, public rlc_metrics_notifier
{
public:
  std::queue<byte_buffer_chain> sdu_queue;
  uint32_t                      sdu_counter = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }
  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

/// Fixture class for RLC TM Rx tests
class rlc_rx_am_test : public ::testing::Test
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

    logger.info("Creating RLC Rx TM entity");

    // Create test frame
    tester = std::make_unique<rlc_rx_tm_test_frame>();

    metrics_agg = std::make_unique<rlc_metrics_aggregator>(
        gnb_du_id_t{}, du_ue_index_t{}, rb_id_t{}, timer_duration{1000}, tester.get(), ue_executor);

    // Create RLC AM TX entity
    rlc = std::make_unique<rlc_rx_tm_entity>(gnb_du_id_t::min,
                                             du_ue_index_t::MIN_DU_UE_INDEX,
                                             srb_id_t::srb0,
                                             make_default_srb0_rlc_config().tm.rx,
                                             *tester,
                                             *metrics_agg,
                                             true,
                                             pcap,
                                             ue_executor,
                                             timers);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&                   logger = srslog::fetch_basic_logger("TEST", false);
  timer_manager                           timers;
  std::unique_ptr<rlc_rx_tm_test_frame>   tester;
  null_rlc_pcap                           pcap;
  std::unique_ptr<rlc_rx_tm_entity>       rlc;
  std::unique_ptr<rlc_metrics_aggregator> metrics_agg;
  manual_task_worker                      ue_executor{128};
};

TEST_F(rlc_rx_am_test, create_new_entity)
{
  EXPECT_EQ(tester->sdu_counter, 0);
}

TEST_F(rlc_rx_am_test, test_rx)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  // write first PDU into lower end
  byte_buffer pdu_buf =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);
  byte_buffer_slice pdu = {pdu_buf.deep_copy().value()};
  rlc->handle_pdu(std::move(pdu));

  count++;

  // write second PDU into lower end
  byte_buffer pdu_buf2 =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);
  pdu = {pdu_buf2.deep_copy().value()};
  rlc->handle_pdu(std::move(pdu));

  // read first SDU from tester
  EXPECT_EQ(tester->sdu_counter, 2);
  byte_buffer_chain& sdu = tester->sdu_queue.front();
  EXPECT_EQ(sdu, pdu_buf);
  tester->sdu_queue.pop();

  // read second SDU from tester
  byte_buffer_chain& sdu2 = tester->sdu_queue.front();
  EXPECT_EQ(sdu2, pdu_buf2);
  tester->sdu_queue.pop();
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
