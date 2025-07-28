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

#include "lib/rlc/rlc_tx_tm_entity.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "srsran/adt/byte_buffer_chain.h"
#include "srsran/rlc/rlc_srb_config_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC TM Tx entity.
class rlc_tx_tm_test_frame : public rlc_tx_upper_layer_data_notifier,
                             public rlc_tx_upper_layer_control_notifier,
                             public rlc_tx_lower_layer_notifier,
                             public rlc_metrics_notifier
{
public:
  std::queue<byte_buffer_slice> sdu_queue;
  uint32_t                      sdu_counter = 0;
  rlc_buffer_state              bsr         = {};
  uint32_t                      bsr_count   = 0;

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn, uint32_t desired_buf_size) override {}
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}
  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override {}
  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(const rlc_buffer_state& bs) override
  {
    this->bsr = bs;
    this->bsr_count++;
  }

  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

/// Fixture class for RLC TM Tx tests
class rlc_tx_tm_test : public ::testing::Test
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

    logger.info("Creating RLC Tx TM entity");

    // Create test frame
    tester = std::make_unique<rlc_tx_tm_test_frame>();

    metrics_coll = std::make_unique<rlc_bearer_metrics_collector>(
        gnb_du_id_t{}, du_ue_index_t{}, rb_id_t{}, timer_duration{1000}, tester.get(), ue_worker);

    // Create RLC TM TX entity
    rlc = std::make_unique<rlc_tx_tm_entity>(gnb_du_id_t::min,
                                             du_ue_index_t::MIN_DU_UE_INDEX,
                                             srb_id_t::srb0,
                                             make_default_srb0_rlc_config().tm.tx,
                                             *tester,
                                             *tester,
                                             *tester,
                                             *metrics_coll,
                                             pcap,
                                             pcell_worker,
                                             ue_worker,
                                             timers);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&                         logger = srslog::fetch_basic_logger("TEST", false);
  timer_manager                                 timers;
  manual_task_worker                            pcell_worker{128};
  manual_task_worker                            ue_worker{128};
  std::unique_ptr<rlc_tx_tm_test_frame>         tester;
  null_rlc_pcap                                 pcap;
  std::unique_ptr<rlc_tx_tm_entity>             rlc;
  std::unique_ptr<rlc_bearer_metrics_collector> metrics_coll;
};

TEST_F(rlc_tx_tm_test, create_new_entity)
{
  rlc_buffer_state bs = rlc->get_buffer_state();
  EXPECT_EQ(bs.pending_bytes, 0);
  EXPECT_FALSE(bs.hol_toa.has_value());
  EXPECT_EQ(tester->bsr.pending_bytes, 0);
  EXPECT_FALSE(tester->bsr.hol_toa.has_value());
  EXPECT_EQ(tester->bsr_count, 0);
}

TEST_F(rlc_tx_tm_test, test_tx)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);

  byte_buffer sdu_buf =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);
  auto t_start = std::chrono::steady_clock::now();
  // write SDU into upper end
  rlc->handle_sdu(sdu_buf.deep_copy().value(), false); // keep local copy for later comparison
  auto t_end = std::chrono::steady_clock::now();
  pcell_worker.run_pending_tasks();
  rlc_buffer_state bs = rlc->get_buffer_state();
  EXPECT_TRUE(bs.hol_toa.has_value());
  EXPECT_GT(bs.hol_toa.value(), t_start);
  EXPECT_LT(bs.hol_toa.value(), t_end);
  EXPECT_EQ(bs.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
  EXPECT_EQ(tester->bsr_count, 1);

  // read PDU from lower end
  std::vector<uint8_t> tx_pdu(sdu_size);
  unsigned             nwritten = rlc->pull_pdu(tx_pdu);
  ue_worker.run_pending_tasks();
  byte_buffer_chain pdu =
      byte_buffer_chain::create(byte_buffer_slice::create(span<uint8_t>(tx_pdu.data(), nwritten)).value()).value();
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf);
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_EQ(bs.pending_bytes, 0);
  EXPECT_FALSE(bs.hol_toa.has_value());
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
  EXPECT_EQ(tester->bsr_count, 1);

  // read another PDU from lower end but there is nothing to read
  tx_pdu.resize(sdu_size);
  nwritten = rlc->pull_pdu(tx_pdu);
  ue_worker.run_pending_tasks();
  pdu = byte_buffer_chain::create(byte_buffer_slice::create(span<uint8_t>(tx_pdu.data(), nwritten)).value()).value();
  EXPECT_EQ(pdu.length(), 0);
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_EQ(bs.pending_bytes, 0);
  EXPECT_FALSE(bs.hol_toa.has_value());
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
  EXPECT_EQ(tester->bsr_count, 1); // unchanged

  // write another SDU into upper end
  count++;
  sdu_buf = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);

  t_start = std::chrono::steady_clock::now();
  rlc->handle_sdu(sdu_buf.deep_copy().value(), false); // keep local copy for later comparison
  t_end = std::chrono::steady_clock::now();
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_TRUE(bs.hol_toa.has_value());
  EXPECT_GT(bs.hol_toa.value(), t_start);
  EXPECT_LT(bs.hol_toa.value(), t_end);
  EXPECT_EQ(bs.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
  EXPECT_EQ(tester->bsr_count, 2);

  // read PDU from lower end with insufficient space for the whole SDU
  tx_pdu.resize(sdu_size - 1);
  nwritten = rlc->pull_pdu(tx_pdu);
  ue_worker.run_pending_tasks();
  pdu = byte_buffer_chain::create(byte_buffer_slice::create(span<uint8_t>(tx_pdu.data(), nwritten)).value()).value();
  EXPECT_EQ(pdu.length(), 0);
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_TRUE(bs.hol_toa.has_value());
  EXPECT_GT(bs.hol_toa.value(), t_start);
  EXPECT_LT(bs.hol_toa.value(), t_end);
  EXPECT_EQ(bs.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged

  // write another SDU into upper end
  count++;
  byte_buffer sdu_buf2 =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);

  // write SDU into upper end
  rlc->handle_sdu(sdu_buf2.deep_copy().value(), false); // keep local copy for later comparison
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_TRUE(bs.hol_toa.has_value());
  EXPECT_GT(bs.hol_toa.value(), t_start); // these are the times from the older SDU
  EXPECT_LT(bs.hol_toa.value(), t_end);   // these are the times from the older SDU
  EXPECT_EQ(bs.pending_bytes, 2 * sdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, 2 * sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start); // these are the times from the older SDU
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);   // these are the times from the older SDU
  EXPECT_EQ(tester->bsr_count, 3);

  // read first PDU from lower end with oversized space
  tx_pdu.resize(3 * sdu_size);
  nwritten = rlc->pull_pdu(tx_pdu);
  ue_worker.run_pending_tasks();
  pdu = byte_buffer_chain::create(byte_buffer_slice::create(span<uint8_t>(tx_pdu.data(), nwritten)).value()).value();
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf);
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_TRUE(bs.hol_toa.has_value());
  EXPECT_GT(bs.hol_toa.value(), t_start); // these are the times from the older SDU
  EXPECT_GT(bs.hol_toa.value(), t_end);   // these are the times from the older SDU - new SDU must be younger
  EXPECT_EQ(bs.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, 2 * sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start); // these are the times from the older SDU
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);   // these are the times from the older SDU
  EXPECT_EQ(tester->bsr_count, 3);

  // read second PDU from lower end with oversized space
  tx_pdu.resize(3 * sdu_size);
  nwritten = rlc->pull_pdu(tx_pdu);
  ue_worker.run_pending_tasks();
  pdu = byte_buffer_chain::create(byte_buffer_slice::create(span<uint8_t>(tx_pdu.data(), nwritten)).value()).value();
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf2);
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_FALSE(bs.hol_toa.has_value());
  EXPECT_EQ(bs.pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, 2 * sdu_size);
  EXPECT_TRUE(tester->bsr.hol_toa.has_value());
  EXPECT_GT(tester->bsr.hol_toa.value(), t_start); // these are the times from the older SDU
  EXPECT_LT(tester->bsr.hol_toa.value(), t_end);   // these are the times from the older SDU
  EXPECT_EQ(tester->bsr_count, 3);
}

TEST_F(rlc_tx_tm_test, discard_sdu_increments_discard_failure_counter)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);

  byte_buffer sdu_buf =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);

  // write SDU into upper end
  rlc->handle_sdu(sdu_buf.deep_copy().value(), false); // keep local copy for later comparison
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // Try discard of valid SDU - but TM does not support any discard, discard failures are counted
  rlc->discard_sdu(0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 0);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 1);

  // read PDU from lower end
  std::vector<uint8_t> tx_pdu(sdu_size);
  unsigned             nwritten = rlc->pull_pdu(tx_pdu);
  ue_worker.run_pending_tasks();
  byte_buffer_chain pdu =
      byte_buffer_chain::create(byte_buffer_slice::create(span<uint8_t>(tx_pdu.data(), nwritten)).value()).value();
  EXPECT_EQ(pdu.length(), sdu_size);
  EXPECT_EQ(pdu, sdu_buf);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);
}

TEST_F(rlc_tx_tm_test, test_tx_metrics)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);

  byte_buffer sdu_buf =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);

  // write SDU into upper end
  rlc->handle_sdu(sdu_buf.deep_copy().value(), false); // keep local copy for later comparison
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // read PDU from lower end
  std::vector<uint8_t> tx_pdu(sdu_size - 1);
  unsigned             nwritten = rlc->pull_pdu(tx_pdu);
  ue_worker.run_pending_tasks();
  byte_buffer_chain pdu =
      byte_buffer_chain::create(byte_buffer_slice::create(span<uint8_t>(tx_pdu.data(), nwritten)).value()).value();

  rlc_tx_metrics m = rlc->get_metrics();
  ASSERT_TRUE(std::holds_alternative<rlc_tm_tx_metrics_lower>(m.tx_low.mode_specific));
  ASSERT_EQ(std::get<rlc_tm_tx_metrics_lower>(m.tx_low.mode_specific).num_small_allocs, 1);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
