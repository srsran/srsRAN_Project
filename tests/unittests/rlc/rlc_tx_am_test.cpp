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

#include "lib/rlc/rlc_tx_am_entity.h"
#include "rlc_test_helpers.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC AM Tx entity.
class rlc_tx_am_test_frame : public rlc_tx_upper_layer_data_notifier,
                             public rlc_tx_upper_layer_control_notifier,
                             public rlc_tx_lower_layer_notifier,
                             public rlc_rx_am_status_provider
{
public:
  std::list<uint32_t> highest_transmitted_pdcp_sn_list;
  std::list<uint32_t> highest_delivered_pdcp_sn_list;
  rlc_am_sn_size      sn_size;
  rlc_am_status_pdu   status;
  bool                status_required = false;
  uint32_t            bsr             = 0;
  uint32_t            bsr_count       = 0;
  uint32_t            max_retx_count  = 0;

  rlc_tx_am_test_frame(rlc_am_sn_size sn_size_) : sn_size(sn_size_), status(sn_size_) {}

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn) override
  {
    // store in list
    highest_transmitted_pdcp_sn_list.push_back(max_tx_pdcp_sn);
  }

  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override
  {
    // store in list
    highest_delivered_pdcp_sn_list.push_back(max_deliv_pdcp_sn);
  }

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override { max_retx_count++; }

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr_) override
  {
    this->bsr = bsr_;
    this->bsr_count++;
  }

  // rlc_rx_am_status_provider interface
  rlc_am_status_pdu get_status_pdu() override { return status; }
  uint32_t          get_status_pdu_length() override { return status.get_packed_size(); }
  bool              status_report_required() override { return status_required; }
};

/// Fixture class for RLC AM Tx tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class rlc_tx_am_test : public ::testing::Test, public ::testing::WithParamInterface<rlc_am_sn_size>, public rlc_trx_test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("RLC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(100);

    logger.info("Creating RLC Tx AM entity ({} bit)", to_number(sn_size));

    // Set Tx config
    config.sn_field_length = sn_size;
    config.t_poll_retx     = 45;
    config.max_retx_thresh = 4;
    config.poll_pdu        = 4;
    config.poll_byte       = 25;

    // Create test frame
    tester = std::make_unique<rlc_tx_am_test_frame>(config.sn_field_length);

    // Create RLC AM TX entity
    rlc = std::make_unique<rlc_tx_am_entity>(du_ue_index_t::MIN_DU_UE_INDEX,
                                             srb_id_t::srb0,
                                             config,
                                             *tester,
                                             *tester,
                                             *tester,
                                             timer_factory{timers, pcell_worker},
                                             pcell_worker,
                                             ue_worker);

    // Bind AM Rx/Tx interconnect
    rlc->set_status_provider(tester.get());
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Creates a new rlc_sdu
  ///
  /// The produced SDU contains an incremental sequence of bytes starting with the value given by first_byte,
  /// i.e. if first_byte = 0xfc, the SDU will be 0xfc 0xfe 0xff 0x00 0x01 ...
  /// \param pdcp_sn PDCP sequence number
  /// \param sdu_size Size of the SDU
  /// \param first_byte Value of the first byte
  /// \return the produced SDU as a rlc_sdu
  rlc_sdu create_rlc_sdu(uint32_t pdcp_sn, uint32_t sdu_size, uint8_t first_byte = 0) const
  {
    rlc_sdu sdu = {create_sdu(sdu_size, first_byte), pdcp_sn};
    return sdu;
  }

  /// \brief Obtains full RLC AMD PDUs from generated SDUs that are passed through an RLC AM entity
  /// \param[out] out_pdus Pre-allocated array of size n_pdus for the resulting RLC AMD PDUs
  /// \param[in] n_pdus Length of the out_pdus array
  /// \param[in] sdu_size Size of SDU that is passed through RLC AM entity
  void tx_full_pdus(byte_buffer_chain* out_pdus, uint32_t n_pdus, uint32_t sdu_size = 1)
  {
    uint32_t n_bsr = tester->bsr_count;

    // Push "n_pdus" SDUs into RLC
    byte_buffer sdu_bufs[n_pdus];
    for (uint32_t i = 0; i < n_pdus; i++) {
      sdu_bufs[i] = create_sdu(sdu_size, i);

      // write SDU into upper end
      rlc_sdu sdu = {sdu_bufs[i].deep_copy(),
                     /* pdcp_sn = */ i}; // no std::move - keep local copy for later comparison
      rlc->handle_sdu(std::move(sdu));
    }
    pcell_worker.run_pending_tasks();
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    uint32_t header_size         = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    uint32_t data_pdu_size       = header_size + sdu_size;
    uint32_t expect_buffer_state = n_pdus * data_pdu_size;

    // Read "n_pdus" PDUs from RLC
    for (uint32_t i = 0; i < n_pdus; i++) {
      EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state - i * data_pdu_size); // actual buffer state changes
      EXPECT_EQ(tester->bsr, expect_buffer_state); // pull_pdu does not push BSR to lower layer
      out_pdus[i] = rlc->pull_pdu(data_pdu_size);
      EXPECT_EQ(out_pdus[i].length(), data_pdu_size);
      EXPECT_TRUE(
          std::equal(out_pdus[i].begin() + header_size, out_pdus[i].end(), sdu_bufs[i].begin(), sdu_bufs[i].end()));
      EXPECT_EQ(tester->bsr_count, n_bsr);
    }
    EXPECT_EQ(rlc->get_buffer_state(), 0);
    EXPECT_EQ(tester->bsr, expect_buffer_state); // pull_pdu does not push BSR to lower layer
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  /// \brief Obtains RLC AMD PDU segments from generated SDUs that are passed through an RLC AM entity
  /// \param[out] out_pdus Pre-allocated array of size n_pdus for the resulting RLC AMD PDU segments
  /// \param[in] n_pdus Length of the out_pdus array
  /// \param[in] pdu_size Maximum size of each PDU that is read from RLC AM entity
  /// \param[in] n_sdus Number of SDUs to push into RLC AM entity
  /// \param[in] sdu_size Size of each SDU that is pushed into RLC AM entity
  void
  tx_segmented_pdus(byte_buffer_chain* out_pdus, uint32_t n_pdus, uint32_t pdu_size, uint32_t n_sdus, uint32_t sdu_size)
  {
    uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    uint32_t header_so_size  = 2;

    uint32_t n_bsr = tester->bsr_count;

    // Precondition
    ASSERT_LT(pdu_size, sdu_size + header_min_size) << "PDU size fits whole SDU; PDUs won't be segmented";

    // Push "n_sdus" SDUs into RLC
    byte_buffer sdu_bufs[n_sdus];
    for (uint32_t i = 0; i < n_sdus; i++) {
      sdu_bufs[i] = create_sdu(sdu_size, i);

      // write SDU into upper end
      rlc_sdu sdu = {sdu_bufs[i].deep_copy(),
                     /* pdcp_sn = */ i}; // no std::move - keep local copy for later comparison
      rlc->handle_sdu(std::move(sdu));
    }
    pcell_worker.run_pending_tasks();
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    uint32_t full_header_size    = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    uint32_t data_pdu_size       = full_header_size + sdu_size;
    uint32_t expect_buffer_state = n_sdus * data_pdu_size;

    // Read "n_pdus" PDUs from RLC
    uint32_t sdu_idx = 0;
    uint32_t sdu_so  = 0;
    for (uint32_t i = 0; i < n_pdus && rlc->get_buffer_state() > 0; i++) {
      uint32_t header_size = header_min_size + (sdu_so == 0 ? 0 : header_so_size);
      out_pdus[i]          = rlc->pull_pdu(pdu_size);

      // Check PDU size
      EXPECT_GT(out_pdus[i].length(), header_size);
      EXPECT_LE(out_pdus[i].length(), pdu_size);

      // Check PDU payload
      auto pdu_begin = out_pdus[i].begin() + header_size;
      auto pdu_end   = out_pdus[i].end();
      auto sdu_begin = sdu_bufs[sdu_idx].begin() + sdu_so;
      ASSERT_TRUE(std::equal(pdu_begin, pdu_end, sdu_begin))
          << "sdu_idx=" << sdu_idx << ", sdu_so=" << sdu_so << ", i=" << i << ", header_size=" << header_size;

      // Check remaining buffer state
      uint32_t rem_sdus      = n_sdus - sdu_idx - 1;
      uint32_t rem_seg_bytes = sdu_bufs[sdu_idx].length() - sdu_so - out_pdus[i].length() + header_size;
      uint32_t rem_seg_hdr   = rem_seg_bytes > 0 ? header_min_size + header_so_size : 0;
      EXPECT_EQ(rlc->get_buffer_state(),
                rem_sdus * (sdu_size + header_min_size) + rem_seg_bytes + rem_seg_hdr); // actual buffer state changes
      EXPECT_EQ(tester->bsr, expect_buffer_state); // pull_pdu does not push BSR to lower layer
      EXPECT_EQ(tester->bsr_count, n_bsr);

      // Update payload offsets
      if (rem_seg_bytes == 0) {
        sdu_idx++;
        sdu_so = 0;
      } else {
        sdu_so += out_pdus[i].length() - header_size;
      }
    }
    EXPECT_EQ(rlc->get_buffer_state(), 0);
    EXPECT_EQ(tester->bsr, expect_buffer_state); // pull_pdu does not push BSR to lower layer
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  void tick()
  {
    timers.tick();
    pcell_worker.run_pending_tasks();
  }

  srslog::basic_logger&                 logger  = srslog::fetch_basic_logger("TEST", false);
  rlc_am_sn_size                        sn_size = GetParam();
  rlc_tx_am_config                      config;
  timer_manager                         timers;
  manual_task_worker                    ue_worker{128};
  manual_task_worker                    pcell_worker{128};
  std::unique_ptr<rlc_tx_am_test_frame> tester;
  std::unique_ptr<rlc_tx_am_entity>     rlc;
};

TEST_P(rlc_tx_am_test, create_new_entity)
{
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), 0);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 0);
  EXPECT_EQ(tester->bsr, 0);
  EXPECT_EQ(tester->bsr_count, 0);
}

TEST_P(rlc_tx_am_test, tx_without_segmentation)
{
  const uint32_t    n_pdus = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, 1);
  tx_full_pdus(pdus, n_pdus, 5);
}

TEST_P(rlc_tx_am_test, tx_small_grant_)
{
  const uint32_t n_sdus   = 5;
  const uint32_t sdu_size = 9;

  const uint32_t n_splits = 3;

  const uint32_t n_pdus      = n_sdus * n_splits;
  const uint32_t header_size = (sn_size == rlc_am_sn_size::size12bits ? 2 : 3);
  const uint32_t so_size     = 2;
  const uint32_t pdu_size    = header_size + so_size + (sdu_size / n_splits);

  byte_buffer_chain pdus[n_pdus];

  tx_segmented_pdus(pdus, n_pdus, pdu_size, n_sdus, sdu_size);
  tx_segmented_pdus(pdus, n_pdus, pdu_size, n_sdus, sdu_size);
}

TEST_P(rlc_tx_am_test, tx_insufficient_space_new_sdu)
{
  const uint32_t sdu_size        = 1;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t short_size      = header_min_size;
  const uint32_t fit_size        = header_min_size + sdu_size;

  EXPECT_EQ(rlc->get_buffer_state(), 0);
  rlc->handle_sdu(create_rlc_sdu(0, sdu_size));
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  byte_buffer_chain pdu;

  // short read - expect empty PDU
  pdu = rlc->pull_pdu(short_size);
  EXPECT_EQ(pdu.length(), 0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // fitting read
  pdu = rlc->pull_pdu(fit_size);
  EXPECT_EQ(pdu.length(), fit_size);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);
}

TEST_P(rlc_tx_am_test, tx_insufficient_space_continued_sdu)
{
  const uint32_t sdu_size        = 3;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t header_so_size  = 2;
  const uint32_t min_size_first  = header_min_size + 1;
  const uint32_t short_size      = header_min_size + header_so_size;
  const uint32_t min_size_seg    = header_min_size + header_so_size + 1;

  EXPECT_EQ(rlc->get_buffer_state(), 0);
  rlc->handle_sdu(create_rlc_sdu(0, sdu_size));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  byte_buffer_chain pdu;

  // kick-off segmentation
  pdu = rlc->pull_pdu(min_size_first);
  EXPECT_EQ(pdu.length(), min_size_first);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), (sdu_size - 1) + header_min_size + header_so_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // short read - expect empty PDU
  pdu = rlc->pull_pdu(short_size);
  EXPECT_EQ(pdu.length(), 0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), (sdu_size - 1) + header_min_size + header_so_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // minimum-length read (middle segment)
  pdu = rlc->pull_pdu(min_size_seg);
  EXPECT_EQ(pdu.length(), min_size_seg);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), (sdu_size - 2) + header_min_size + header_so_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // minimum-length read (last segment)
  pdu = rlc->pull_pdu(min_size_seg);
  EXPECT_EQ(pdu.length(), min_size_seg);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);
}

TEST_P(rlc_tx_am_test, sdu_discard)
{
  const uint32_t sdu_size = 1;
  const uint32_t n_pdus   = 6;
  uint32_t       n_bsr    = tester->bsr_count;

  // Push "n_pdus" SDUs into RLC
  byte_buffer sdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    sdu_bufs[i] = create_sdu(sdu_size, i);

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(),
                   /* pdcp_sn = */ i}; // no std::move - keep local copy for later comparison
    rlc->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // reset
  tester->bsr_count = 0;
  n_bsr             = 0;

  // Discard valid SDUs
  rlc->discard_sdu(0);
  rlc->discard_sdu(4); // out-of-order discard
  rlc->discard_sdu(3);

  // Compute expected buffer state
  uint32_t header_size         = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  uint32_t data_pdu_size       = header_size + sdu_size;
  uint32_t expect_buffer_state = (n_pdus - 3) * data_pdu_size;
  uint32_t expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 0);

  // Try discard of invalid SDU
  rlc->discard_sdu(999);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 1);

  // Try discard of already discarded SDU
  rlc->discard_sdu(0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 2);

  // Transmit full PDU
  byte_buffer_chain pdu;
  pdu = rlc->pull_pdu(data_pdu_size);
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[1].begin()));
  pcell_worker.run_pending_tasks();
  expect_buffer_state = (n_pdus - 4) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 2);

  // Try discard of already transmitted SDU
  rlc->discard_sdu(1);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 3);

  // Transmit full PDU
  pdu = rlc->pull_pdu(data_pdu_size);
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[2].begin()));
  expect_buffer_state = (n_pdus - 5) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 3);

  // Discard remaining SDU
  rlc->discard_sdu(5);
  expect_buffer_state = 0;
  expect_mac_bsr      = 0;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 4);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 3);
}

TEST_P(rlc_tx_am_test, sdu_discard_with_pdcp_sn_wraparound)
{
  const uint32_t sdu_size = 1;
  const uint32_t n_pdus   = 6;
  uint32_t       n_bsr    = tester->bsr_count;

  const uint32_t pdcp_sn_start = 4092;
  const uint32_t pdcp_sn_mod   = 4096;

  // Push "n_pdus" SDUs into RLC
  byte_buffer sdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    sdu_bufs[i] = create_sdu(sdu_size, i);

    // write SDU into upper end
    rlc_sdu sdu = {sdu_bufs[i].deep_copy(),
                   /* pdcp_sn = */ (pdcp_sn_start + i) % pdcp_sn_mod}; // no std::move, keep copy for later comparison
    rlc->handle_sdu(std::move(sdu));
  }
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // reset
  tester->bsr_count = 0;
  n_bsr             = 0;

  // Discard valid SDUs
  rlc->discard_sdu((pdcp_sn_start + 0) % pdcp_sn_mod);
  rlc->discard_sdu((pdcp_sn_start + 4) % pdcp_sn_mod); // post-wraparound discard
  rlc->discard_sdu((pdcp_sn_start + 3) % pdcp_sn_mod); // out-of-order discard (pre-wraparound)

  // Compute expected buffer state
  uint32_t header_size         = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  uint32_t data_pdu_size       = header_size + sdu_size;
  uint32_t expect_buffer_state = (n_pdus - 3) * data_pdu_size;
  uint32_t expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;

  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 0);

  // Try discard of invalid SDU
  rlc->discard_sdu((pdcp_sn_start + 999) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 1);

  // Try discard of already discarded SDU
  rlc->discard_sdu((pdcp_sn_start + 0) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 2);

  // Transmit full PDU
  byte_buffer_chain pdu;
  pdu = rlc->pull_pdu(data_pdu_size);
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[1].begin()));
  expect_buffer_state = (n_pdus - 4) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 2);

  // Try discard of already transmitted SDU
  rlc->discard_sdu((pdcp_sn_start + 1) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 3);

  // Transmit full PDU
  pdu = rlc->pull_pdu(data_pdu_size);
  EXPECT_FALSE(pdu.empty());
  EXPECT_TRUE(std::equal(pdu.begin() + header_size, pdu.end(), sdu_bufs[2].begin()));
  expect_buffer_state = (n_pdus - 5) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 3);

  // Discard remaining SDU
  rlc->discard_sdu((pdcp_sn_start + 5) % pdcp_sn_mod);
  expect_buffer_state = 0;
  expect_mac_bsr      = 0;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), expect_buffer_state);
  EXPECT_EQ(tester->bsr, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().num_discarded_sdus, 4);
  EXPECT_EQ(rlc->get_metrics().num_discard_failures, 3);
}

TEST_P(rlc_tx_am_test, retx_pdu_without_segmentation)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx as full PDU
  byte_buffer_chain retx_pdu;
  retx_pdu = rlc->pull_pdu(sdu_size + header_min_size);
  logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_EQ(retx_pdu, pdus[nack.nack_sn]);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_with_segmentation)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    so_size         = 2;
  const uint32_t    header_max_size = header_min_size + so_size;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx in segments
  byte_buffer_chain retx_pdu;
  for (uint32_t i = 0; i < sdu_size; i++) {
    uint32_t header_size = i == 0 ? header_min_size : header_max_size;
    retx_pdu             = rlc->pull_pdu(1 + header_size);
    logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
    logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
    EXPECT_TRUE(
        std::equal(retx_pdu.begin() + header_size, retx_pdu.end(), pdus[nack.nack_sn].begin() + header_min_size + i));
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), 5);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_first_segment_without_segmentation)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3 0:1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 0;
  nack.so_end             = 1;
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), nack.so_end - nack.so_start + 1 + header_min_size);
  EXPECT_EQ(tester->bsr, nack.so_end - nack.so_start + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  byte_buffer_chain retx_pdu;
  retx_pdu = rlc->pull_pdu(sdu_size + header_min_size);
  logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(
      std::equal(retx_pdu.begin() + header_min_size, retx_pdu.end(), pdus[nack.nack_sn].begin() + header_min_size));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, nack.so_end - nack.so_start + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_middle_segment_without_segmentation)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    so_size         = 2;
  const uint32_t    header_max_size = header_min_size + so_size;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3 1:1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 1;
  nack.so_end             = 1;
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  byte_buffer_chain retx_pdu;
  retx_pdu = rlc->pull_pdu(sdu_size + header_max_size);
  logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(std::equal(retx_pdu.begin() + header_max_size,
                         retx_pdu.end(),
                         pdus[nack.nack_sn].begin() + header_min_size + nack.so_start));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_last_segment_without_segmentation)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    so_size         = 2;
  const uint32_t    header_max_size = header_min_size + so_size;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3 1:2
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 1;
  nack.so_end             = 2;
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  byte_buffer_chain retx_pdu;
  retx_pdu = rlc->pull_pdu(sdu_size + header_max_size);
  logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(std::equal(retx_pdu.begin() + header_max_size,
                         retx_pdu.end(),
                         pdus[nack.nack_sn].begin() + header_min_size + nack.so_start));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_segment_invalid_so_start_and_so_end)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3 3:3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 3; // out of bounds
  nack.so_end             = 3; // out of bounds
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  byte_buffer_chain retx_pdu;
  retx_pdu = rlc->pull_pdu(sdu_size + header_min_size);
  logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(
      std::equal(retx_pdu.begin() + header_min_size, retx_pdu.end(), pdus[nack.nack_sn].begin() + header_min_size));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_segment_invalid_so_start_larger_than_so_end)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3 3:2
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 2; // larger than so_end
  nack.so_end             = 1; // smaller than so_start
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), nack.so_end + 1 + header_min_size);
  EXPECT_EQ(tester->bsr, nack.so_end + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  byte_buffer_chain retx_pdu;
  retx_pdu = rlc->pull_pdu(sdu_size + header_min_size);
  logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(
      std::equal(retx_pdu.begin() + header_min_size, retx_pdu.end(), pdus[nack.nack_sn].begin() + header_min_size));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, nack.so_end + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, invalid_nack_nack_sn_outside_rx_window)
{
  const uint32_t    sdu_size = 3;
  const uint32_t    n_pdus   = 12;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);

  // ACK up until 5
  rlc_am_status_pdu status_pdu0(sn_size);
  status_pdu0.ack_sn = 5;
  rlc->on_status_pdu(status_pdu0);

  // Status PDU 1
  rlc_am_status_pdu status_pdu1(sn_size);
  status_pdu1.ack_sn = 8;
  {
    rlc_am_status_nack nack = {};
    nack.nack_sn            = 6;
    nack.has_nack_range     = true;
    nack.nack_range         = 2;
    status_pdu1.push_nack(nack);
  }

  // Status PDU 2
  rlc_am_status_pdu status_pdu2(sn_size);
  {
    status_pdu2.ack_sn      = 8;
    rlc_am_status_nack nack = {};
    nack.nack_sn            = 7;
    status_pdu2.push_nack(nack);
  }

  // Receive 2 before 1
  rlc->on_status_pdu(status_pdu2);
  ASSERT_NE(rlc->get_buffer_state(), 0); // should have one PDU to RETX
  byte_buffer_chain retx_pdu;
  rlc->pull_pdu(rlc->get_buffer_state());
  rlc_tx_am_state st0 = rlc->get_state();
  ASSERT_EQ(st0.tx_next_ack, 7); // TX_NEXT_ACK should be the same as NACK_SN now

  // Receive 1 now
  rlc->on_status_pdu(status_pdu1);       // NACK SN=6 is now outside TX window
  ASSERT_EQ(rlc->get_buffer_state(), 0); // should have ignored status report
  rlc_tx_am_state st1 = rlc->get_state();
  ASSERT_EQ(st1.tx_next_ack, 7); // TX_NEXT_ACK should have not changed
}

TEST_P(rlc_tx_am_test, invalid_nack_sn_larger_than_ack_sn)
{
  const uint32_t    sdu_size = 3;
  const uint32_t    n_pdus   = 12;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);

  // ACK up until 5
  rlc_am_status_pdu status_pdu0(sn_size);
  status_pdu0.ack_sn = 5;
  rlc->on_status_pdu(status_pdu0);

  // Status PDU 1
  rlc_am_status_pdu status_pdu1(sn_size);
  status_pdu1.ack_sn = 8;
  {
    rlc_am_status_nack nack = {};
    nack.nack_sn            = 8;
    nack.has_nack_range     = false;
    status_pdu1.push_nack(nack);
  }

  // Receive Status 1
  rlc->on_status_pdu(status_pdu1);
  ASSERT_EQ(rlc->get_buffer_state(), 0); // should have ignored status report
  rlc_tx_am_state st1 = rlc->get_state();
  ASSERT_EQ(st1.tx_next_ack, 5); // TX_NEXT_ACK should have not changed

  // Status PDU 2
  rlc_am_status_pdu status_pdu2(sn_size);
  status_pdu2.ack_sn = 8;
  {
    rlc_am_status_nack nack = {};
    nack.nack_sn            = 5;
    nack.has_nack_range     = true;
    nack.nack_range         = 3; // NACK 5, 6 and 7, should be valid
    status_pdu2.push_nack(nack);
  }

  // Receive Status 2
  rlc->on_status_pdu(status_pdu2);
  ASSERT_NE(rlc->get_buffer_state(), 0);  // should have processed status report
  rlc->pull_pdu(rlc->get_buffer_state()); // RETX 5
  rlc->pull_pdu(rlc->get_buffer_state()); // RETX 6
  rlc->pull_pdu(rlc->get_buffer_state()); // RETX 7
  ASSERT_EQ(rlc->get_buffer_state(), 0);
  rlc_tx_am_state st2 = rlc->get_state();
  ASSERT_EQ(st2.tx_next_ack, 5); // TX_NEXT_ACK should have not changed

  // Status PDU 3
  rlc_am_status_pdu status_pdu3(sn_size);
  status_pdu3.ack_sn = 8;
  {
    rlc_am_status_nack nack = {};
    nack.nack_sn            = 6;
    nack.has_nack_range     = true;
    nack.nack_range         = 3; // NACK 6, 7 and 8, should be invalid
    status_pdu3.push_nack(nack);
  }

  // Receive Status 3
  rlc->on_status_pdu(status_pdu3);
  ASSERT_EQ(rlc->get_buffer_state(), 0); // should have dropped status report
  rlc_tx_am_state st3 = rlc->get_state();
  ASSERT_EQ(st3.tx_next_ack, 5); // TX_NEXT_ACK should have not changed
}

TEST_P(rlc_tx_am_test, retx_insufficient_space)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    short_size      = header_min_size;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // short read of ReTx
  byte_buffer_chain retx_pdu;
  retx_pdu = rlc->pull_pdu(short_size);
  EXPECT_EQ(retx_pdu.length(), 0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_range_without_segmentation)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  uint32_t sn_start = cardinality(to_number(sn_size)) - 2;
  uint32_t nack_sn  = sn_start + 1;
  uint32_t range    = 3;

  // Configure state to upper edge
  rlc_tx_am_state st = {};
  st.tx_next         = sn_start;
  st.tx_next_ack     = sn_start;
  st.poll_sn         = sn_start;
  rlc->set_state(st);

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=nack_sn..nack_sn+range-1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = nack_sn;
  nack.has_nack_range     = true;
  nack.nack_range         = range;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = (sn_start + n_pdus) % cardinality(to_number(sn_size));
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr, range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx as full PDU
  for (uint32_t i = nack_sn; i < nack_sn + range; i++) {
    byte_buffer_chain retx_pdu;
    retx_pdu = rlc->pull_pdu(sdu_size + header_min_size);
    logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
    logger.debug(pdus[i - sn_start].begin(), pdus[i - sn_start].end(), "pdus[{}]:", i - sn_start);
    EXPECT_TRUE(std::equal(retx_pdu.begin() + 1, retx_pdu.end(), pdus[i - sn_start].begin() + 1)); // skip header P bit
    EXPECT_EQ(rlc->get_buffer_state(), (nack_sn + range - 1 - i) * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr, range * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 0);
}

TEST_P(rlc_tx_am_test, retx_pdu_range_wraparound)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  uint32_t sn_start = cardinality(to_number(sn_size)) - 2;
  uint32_t nack_sn  = sn_start + 1;
  uint32_t range    = 4; // maximum valid range is 4

  // Configure state to upper edge
  rlc_tx_am_state st = {};
  st.tx_next         = sn_start;
  st.tx_next_ack     = sn_start;
  st.poll_sn         = sn_start;
  rlc->set_state(st);

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=nack_sn..nack_sn+range-1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = nack_sn;
  nack.has_nack_range     = true;
  nack.nack_range         = range;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = (sn_start + n_pdus) % cardinality(to_number(sn_size));
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(status_pdu);
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr, range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx as full PDU
  for (uint32_t i = nack_sn; i < nack_sn + range; i++) {
    byte_buffer_chain retx_pdu;
    retx_pdu = rlc->pull_pdu(sdu_size + header_min_size);
    logger.debug(retx_pdu.begin(), retx_pdu.end(), "retx_pdu:");
    logger.debug(pdus[i - sn_start].begin(), pdus[i - sn_start].end(), "pdus[{}]:", i - sn_start);
    ASSERT_EQ(retx_pdu.length(), pdus[i - sn_start].length());
    // compare PDUs, ignore P bit in first header byte
    EXPECT_EQ(*retx_pdu.begin() & 0x4, *pdus[i - sn_start].begin() & 0x4);                         // mask P bit
    EXPECT_TRUE(std::equal(retx_pdu.begin() + 1, retx_pdu.end(), pdus[i - sn_start].begin() + 1)); // check rest
    EXPECT_EQ(rlc->get_buffer_state(), (nack_sn + range - 1 - i) * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr, range * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 0);
}

TEST_P(rlc_tx_am_test, buffer_state_considers_status_report)
{
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), 0);

  // First set the status_required flag without triggering the on_status_report_changed() event
  tester->status_required = true;
  EXPECT_EQ(rlc->get_buffer_state(), tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr, 0);       // unchanged
  EXPECT_EQ(tester->bsr_count, 0); // unchanged

  rlc_am_status_nack nack = {};
  nack.has_nack_range     = true;
  nack.has_so             = true;
  tester->status.push_nack(nack);
  EXPECT_EQ(rlc->get_buffer_state(), tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr, 0);       // unchanged
  EXPECT_EQ(tester->bsr_count, 0); // unchanged

  // Now trigger on_status_report_changed() as well and check for the expected outcomes
  rlc->on_status_report_changed();
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr, tester->status.get_packed_size()); // expecting the size of the status report
  EXPECT_EQ(tester->bsr_count, 1);                          // a bsr should be sent to lower layers

  // Now unflag status_required (RX entity does so when a status report was picked); this shouldn't trigger anything
  tester->status_required = false;
  EXPECT_EQ(rlc->get_buffer_state(), 0);
  EXPECT_EQ(tester->bsr, tester->status.get_packed_size()); // unchanged
  EXPECT_EQ(tester->bsr_count, 1);                          // unchanged
}

TEST_P(rlc_tx_am_test, status_report_priority)
{
  // First push one SDU into RLC to also check precedence of status report
  const uint32_t sdu_size        = 1;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  EXPECT_EQ(rlc->get_buffer_state(), 0);
  rlc->handle_sdu(create_rlc_sdu(0, sdu_size));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), pdu_size);
  EXPECT_EQ(tester->bsr, pdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // Set status report required
  tester->status_required = true;
  rlc->on_status_report_changed();
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr, tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr_count, 2);

  tester->status.ack_sn   = 20;
  rlc_am_status_nack nack = {};

  nack.nack_sn = 5;
  tester->status.push_nack(nack);
  nack.nack_sn = 10;
  tester->status.push_nack(nack);
  nack.nack_sn = 15;
  tester->status.push_nack(nack);
  EXPECT_EQ(rlc->get_buffer_state(), tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged

  byte_buffer_chain pdu;

  // Read PDU, expect to be the status PDU
  pdu = rlc->pull_pdu(tester->status.get_packed_size());
  EXPECT_EQ(pdu.length(), tester->status.get_packed_size());

  // The tester does not unflag status_required automatically, hence rlc still assumes that status PDU is needed
  EXPECT_EQ(rlc->get_buffer_state(), tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged

  // Disable status PDU requirement and check there is only the SDU waiting for Tx; this shouldn't trigger anything
  tester->status_required = false;
  EXPECT_EQ(rlc->get_buffer_state(), pdu_size);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged
}

TEST_P(rlc_tx_am_test, status_report_trim)
{
  EXPECT_EQ(rlc->get_buffer_state(), 0);

  // Set status report required
  tester->status_required = true;
  rlc->on_status_report_changed();
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr, tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr_count, 1);

  // Create a dummy status PDU with a few NACKs
  tester->status.ack_sn   = 20;
  rlc_am_status_nack nack = {};

  nack.nack_sn = 5;
  tester->status.push_nack(nack);
  nack.nack_sn = 10;
  tester->status.push_nack(nack);

  // Store current size so far
  uint32_t trimmed_size = tester->status.get_packed_size();

  // Append further NACKs that will be trimmed
  nack.nack_sn = 15;
  tester->status.push_nack(nack);
  rlc->on_status_report_changed();
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr, tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr_count, 2);

  byte_buffer_chain pdu;

  // Fail to read status PDU due to insufficient grant size to fit a totally trimmed status PDU
  pdu = rlc->pull_pdu(rlc_am_nr_status_pdu_sizeof_header_ack_sn - 1);
  EXPECT_EQ(pdu.length(), 0);

  // Read trimmed status PDU (short by 1 byte, i.e. last NACK will be trimmed)
  pdu = rlc->pull_pdu(tester->status.get_packed_size() - 1);
  EXPECT_EQ(pdu.length(), trimmed_size);

  // Read untrimmed status PDU
  pdu = rlc->pull_pdu(tester->status.get_packed_size());
  EXPECT_EQ(pdu.length(), tester->status.get_packed_size());
}

TEST_P(rlc_tx_am_test, expired_poll_retransmit_timer_triggers_retx)
{
  const uint32_t    n_pdus = 5;
  byte_buffer_chain pdus[n_pdus];

  const uint32_t sdu_size        = 10;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t expect_mac_bsr = tester->bsr;
  uint32_t n_bsr          = tester->bsr_count;

  // advance timers to expire poll_retransmit_timer
  for (int i = 0; i < config.t_poll_retx; i++) {
    EXPECT_EQ(rlc->get_buffer_state(), 0);
    EXPECT_EQ(tester->bsr, expect_mac_bsr);
    EXPECT_EQ(tester->bsr_count, n_bsr);
    tick();
  }

  // expiry of poll_retransmit_timer should schedule an SDU for ReTx
  EXPECT_EQ(rlc->get_buffer_state(), pdu_size);
  EXPECT_EQ(tester->bsr, pdu_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  {
    // pull one segment but leave 2 bytes for later.
    // check if the polling (P) bit IS set in the PDU header (because of previously expired poll_retransmit_timer)
    byte_buffer_chain pdu     = rlc->pull_pdu(rlc->get_buffer_state() - 2);
    rlc_am_pdu_header pdu_hdr = {};
    rlc_am_read_data_pdu_header(byte_buffer(pdu.begin(), pdu.end()), sn_size, &pdu_hdr);
    EXPECT_TRUE(pdu_hdr.p);
    EXPECT_EQ(tester->bsr, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  {
    // pull next segment but leave 1 byte for later.
    // check if the polling (P) bit is NOT set anymore in the PDU header (non-empty queues and timer not expired again)
    byte_buffer_chain pdu     = rlc->pull_pdu(rlc->get_buffer_state() - 1);
    rlc_am_pdu_header pdu_hdr = {};
    rlc_am_read_data_pdu_header(byte_buffer(pdu.begin(), pdu.end()), sn_size, &pdu_hdr);
    EXPECT_FALSE(pdu_hdr.p);
    EXPECT_EQ(tester->bsr, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  {
    // pull final segment so that RLC queues run empty.
    // check if the polling (P) bit IS set anymore in the PDU header (because RLC queues are run empty)
    byte_buffer_chain pdu     = rlc->pull_pdu(rlc->get_buffer_state());
    rlc_am_pdu_header pdu_hdr = {};
    rlc_am_read_data_pdu_header(byte_buffer(pdu.begin(), pdu.end()), sn_size, &pdu_hdr);
    EXPECT_TRUE(pdu_hdr.p);
    EXPECT_EQ(tester->bsr, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }
}

TEST_P(rlc_tx_am_test, expired_poll_retransmit_timer_sets_polling_bit)
{
  const uint32_t    n_pdus = 5;
  byte_buffer_chain pdus[n_pdus];

  const uint32_t sdu_size        = 10;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  tx_full_pdus(pdus, n_pdus, sdu_size);

  // push SDU to SDU queue so that it is not empty
  uint32_t    n_bsr   = tester->bsr_count;
  byte_buffer sdu_buf = create_sdu(sdu_size, 7);
  rlc_sdu     sdu     = {sdu_buf.deep_copy(), /* pdcp_sn = */ 7}; // no std::move - keep local copy for later comparison
  rlc->handle_sdu(std::move(sdu));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state(), pdu_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(tester->bsr, pdu_size);

  {
    // pull one segment but leave 3 bytes for later.
    // check if the polling (P) bit is NOT set in the PDU header
    byte_buffer_chain pdu     = rlc->pull_pdu(rlc->get_buffer_state() - 3);
    rlc_am_pdu_header pdu_hdr = {};
    rlc_am_read_data_pdu_header(byte_buffer(pdu.begin(), pdu.end()), sn_size, &pdu_hdr);
    EXPECT_FALSE(pdu_hdr.p);
  }

  // advance timers to expire poll_retransmit_timer
  uint32_t old_bsr = rlc->get_buffer_state();
  for (int i = 0; i < config.t_poll_retx; i++) {
    EXPECT_EQ(rlc->get_buffer_state(), old_bsr);
    EXPECT_EQ(tester->bsr, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
    tick();
  }

  // expiry of poll_retransmit_timer should not schedule any extra SDU for ReTx, since SDU queue is not empty
  EXPECT_EQ(tester->bsr, pdu_size);
  EXPECT_EQ(rlc->get_buffer_state(), old_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  {
    // pull next segment but leave 2 bytes for later.
    // check if the polling (P) bit IS set in the PDU header (because of previously expired poll_retransmit_timer)
    byte_buffer_chain pdu     = rlc->pull_pdu(rlc->get_buffer_state() - 2);
    rlc_am_pdu_header pdu_hdr = {};
    rlc_am_read_data_pdu_header(byte_buffer(pdu.begin(), pdu.end()), sn_size, &pdu_hdr);
    EXPECT_TRUE(pdu_hdr.p);
  }

  {
    // pull next segment but leave 1 byte for later.
    // check if the polling (P) bit is NOT set anymore in the PDU header (non-empty queues and timer not expired again)
    byte_buffer_chain pdu     = rlc->pull_pdu(rlc->get_buffer_state() - 1);
    rlc_am_pdu_header pdu_hdr = {};
    rlc_am_read_data_pdu_header(byte_buffer(pdu.begin(), pdu.end()), sn_size, &pdu_hdr);
    EXPECT_FALSE(pdu_hdr.p);
  }

  {
    // pull final segment so that RLC queues run empty.
    // check if the polling (P) bit IS set anymore in the PDU header (because RLC queues are run empty)
    byte_buffer_chain pdu     = rlc->pull_pdu(rlc->get_buffer_state());
    rlc_am_pdu_header pdu_hdr = {};
    rlc_am_read_data_pdu_header(byte_buffer(pdu.begin(), pdu.end()), sn_size, &pdu_hdr);
    EXPECT_TRUE(pdu_hdr.p);
  }
}

TEST_P(rlc_tx_am_test, retx_count_ignores_pending_retx)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  for (uint32_t n_retx = 0; n_retx <= config.max_retx_thresh; n_retx++) {
    // Expect that no max_retx has been reached
    EXPECT_EQ(tester->max_retx_count, 0);

    // NACK SN=1 and SN=3
    rlc_am_status_nack nack1 = {};
    nack1.nack_sn            = 1;
    rlc_am_status_nack nack3 = {};
    nack3.nack_sn            = 3;
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = n_pdus;
    status_pdu.push_nack(nack1);
    status_pdu.push_nack(nack3);
    rlc->on_status_pdu(status_pdu);
    pcell_worker.run_pending_tasks();
    ue_worker.run_pending_tasks();
    EXPECT_EQ(rlc->get_buffer_state(), 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr, 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    // Do not read both ReTx
  }

  // Expect that no max_retx has been reached
  EXPECT_EQ(tester->max_retx_count, 0);
}

TEST_P(rlc_tx_am_test, retx_count_trigger_max_retx_without_segmentation)
{
  const uint32_t    sdu_size        = 3;
  const uint32_t    header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t    n_pdus          = 5;
  byte_buffer_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, sdu_size);
  uint32_t n_bsr = tester->bsr_count;

  for (uint32_t n_retx = 0; n_retx <= config.max_retx_thresh; n_retx++) {
    // Expect that no max_retx has been reached
    EXPECT_EQ(tester->max_retx_count, 0);

    // NACK SN=1 and SN=3
    rlc_am_status_nack nack1 = {};
    nack1.nack_sn            = 1;
    rlc_am_status_nack nack3 = {};
    nack3.nack_sn            = 3;
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = n_pdus;
    status_pdu.push_nack(nack1);
    status_pdu.push_nack(nack3);
    rlc->on_status_pdu(status_pdu);
    pcell_worker.run_pending_tasks();
    ue_worker.run_pending_tasks();
    EXPECT_EQ(rlc->get_buffer_state(), 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr, 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    // Read both ReTx as full PDUs
    byte_buffer_chain retx_pdu1;
    retx_pdu1 = rlc->pull_pdu(sdu_size + header_min_size);
    EXPECT_EQ(retx_pdu1.length(), (sdu_size + header_min_size));
    byte_buffer_chain retx_pdu3;
    retx_pdu3 = rlc->pull_pdu(sdu_size + header_min_size);
    EXPECT_EQ(retx_pdu3.length(), (sdu_size + header_min_size));
    EXPECT_EQ(rlc->get_buffer_state(), 0);
  }

  // Finally, max_retx has been reached for both SNs
  EXPECT_EQ(tester->max_retx_count, 2);
}

///////////////////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size
///////////////////////////////////////////////////////////////////////////////

std::string test_param_info_to_string(const ::testing::TestParamInfo<rlc_am_sn_size>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}bit", to_number(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(rlc_tx_am_test_each_sn_size,
                         rlc_tx_am_test,
                         ::testing::Values(rlc_am_sn_size::size12bits, rlc_am_sn_size::size18bits),
                         test_param_info_to_string);
