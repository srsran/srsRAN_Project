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

#include "lib/rlc/rlc_tx_am_entity.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/rtsan.h"
#include <gtest/gtest.h>
#include <list>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC AM Tx entity.
class rlc_tx_am_test_frame : public rlc_tx_upper_layer_data_notifier,
                             public rlc_tx_upper_layer_control_notifier,
                             public rlc_tx_lower_layer_notifier,
                             public rlc_rx_am_status_provider,
                             public rlc_metrics_notifier
{
public:
  std::list<uint32_t> highest_transmitted_pdcp_sn_list;
  std::list<uint32_t> highest_delivered_pdcp_sn_list;
  std::list<uint32_t> highest_retransmitted_pdcp_sn_list;
  std::list<uint32_t> highest_delivered_retransmitted_pdcp_sn_list;
  std::list<uint32_t> desired_buf_size_list;
  rlc_am_sn_size      sn_size;
  rlc_am_status_pdu   status;
  bool                status_required = false;
  rlc_buffer_state    bsr             = {};
  uint32_t            bsr_count       = 0;
  uint32_t            max_retx_count  = 0;
  uint32_t            proto_err_count = 0;

  rlc_tx_am_test_frame(rlc_am_sn_size sn_size_) : sn_size(sn_size_), status(sn_size_) {}

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn, uint32_t desired_buf_size) override
  {
    SRSRAN_RTSAN_SCOPED_DISABLER(d);
    // store in list
    highest_transmitted_pdcp_sn_list.push_back(max_tx_pdcp_sn);
    desired_buf_size_list.push_back(desired_buf_size);
  }

  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override
  {
    SRSRAN_RTSAN_SCOPED_DISABLER(d);
    // store in list
    highest_delivered_pdcp_sn_list.push_back(max_deliv_pdcp_sn);
  }

  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override
  {
    SRSRAN_RTSAN_SCOPED_DISABLER(d);
    // store in list
    highest_retransmitted_pdcp_sn_list.push_back(max_retx_pdcp_sn);
  }

  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override
  {
    SRSRAN_RTSAN_SCOPED_DISABLER(d);
    // store in list
    highest_delivered_retransmitted_pdcp_sn_list.push_back(max_deliv_retx_pdcp_sn);
  }

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override { proto_err_count++; }
  void on_max_retx() override { max_retx_count++; }

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(const rlc_buffer_state& bs) override
  {
    this->bsr = bs;
    this->bsr_count++;
  }

  // rlc_rx_am_status_provider interface
  rlc_am_status_pdu& get_status_pdu() override { return status; }
  uint32_t           get_status_pdu_length() override { return status.get_packed_size(); }
  bool               status_report_required() override { return status_required; }

  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

/// Fixture class for RLC AM Tx tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class rlc_tx_am_test : public ::testing::Test, public ::testing::WithParamInterface<rlc_am_sn_size>
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
    config.sn_field_length  = sn_size;
    config.pdcp_sn_len      = static_cast<pdcp_sn_size>(sn_size); // use the same SN size for PDCP
    config.t_poll_retx      = 45;
    config.max_retx_thresh  = 4;
    config.poll_pdu         = 4;
    config.poll_byte        = 25;
    config.max_window       = 0;
    config.queue_size       = 4096;
    config.queue_size_bytes = 4096 * 1500;

    // Create test frame
    tester = std::make_unique<rlc_tx_am_test_frame>(config.sn_field_length);

    metrics_coll = std::make_unique<rlc_bearer_metrics_collector>(
        gnb_du_id_t{}, du_ue_index_t{}, rb_id_t{}, timer_duration{1000}, tester.get(), ue_worker);

    // Create RLC AM TX entity
    rlc = std::make_unique<rlc_tx_am_entity>(gnb_du_id_t::min,
                                             du_ue_index_t::MIN_DU_UE_INDEX,
                                             drb_id_t::drb1,
                                             config,
                                             *tester,
                                             *tester,
                                             *tester,
                                             *metrics_coll,
                                             pcap,
                                             pcell_worker,
                                             ue_worker,
                                             timers);

    // Bind AM Rx/Tx interconnect
    rlc->set_status_provider(tester.get());
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Obtains full RLC AMD PDUs from generated SDUs that are passed through an RLC AM entity
  /// \param[in] n_pdus Length of the out_pdus array
  /// \param[in] sdu_size Size of SDU that is passed through RLC AM entity
  /// \return A vector of resulting RLC AMD PDUs
  std::vector<std::vector<uint8_t>> tx_full_pdus(uint32_t n_pdus, uint32_t sdu_size = 1)
  {
    std::vector<std::vector<uint8_t>> out_pdus;
    out_pdus.reserve(n_pdus);
    uint32_t n_bsr = tester->bsr_count;

    // Push "n_pdus" SDUs into RLC
    auto sdu_bufs = std::vector<byte_buffer>(n_pdus);
    auto t_start  = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < n_pdus; i++) {
      sdu_bufs[i] = test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, i, sdu_size, i);

      // write SDU into upper end
      rlc->handle_sdu(sdu_bufs[i].deep_copy().value(), false); // keep local copy for later comparison
    }
    auto t_end = std::chrono::steady_clock::now();
    pcell_worker.run_pending_tasks();
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    uint32_t header_size         = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    uint32_t data_pdu_size       = header_size + sdu_size;
    uint32_t expect_buffer_state = n_pdus * data_pdu_size;

    // Read "n_pdus" PDUs from RLC
    for (uint32_t i = 0; i < n_pdus; i++) {
      rlc_buffer_state bs = rlc->get_buffer_state();
      EXPECT_EQ(bs.pending_bytes,
                expect_buffer_state - i * data_pdu_size); // actual buffer state changes
      EXPECT_TRUE(bs.hol_toa.has_value());
      EXPECT_GT(bs.hol_toa.value(), t_start);
      EXPECT_LT(bs.hol_toa.value(), t_end);
      EXPECT_EQ(tester->bsr.pending_bytes, expect_buffer_state); // pull_pdu does not push BSR to lower layer
      EXPECT_TRUE(tester->bsr.hol_toa.has_value());
      EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
      EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
      std::vector<uint8_t> pdu_buf;
      pdu_buf.resize(data_pdu_size);
      size_t pdu_len = rlc->pull_pdu(pdu_buf);
      pdu_buf.resize(pdu_len);
      out_pdus.push_back(std::move(pdu_buf));

      // Check PDU size
      EXPECT_EQ(out_pdus[i].size(), data_pdu_size);
      // Check PDU payload
      EXPECT_TRUE(
          std::equal(out_pdus[i].begin() + header_size, out_pdus[i].end(), sdu_bufs[i].begin(), sdu_bufs[i].end()));
      // Check remaining buffer state
      EXPECT_EQ(tester->bsr_count, n_bsr);

      // Check SI
      rlc_si_field si, si_expect;
      si        = static_cast<rlc_si_field>((*out_pdus[i].begin() >> 4) & 0b11);
      si_expect = rlc_si_field::full_sdu;
      EXPECT_EQ(si, si_expect);
    }
    rlc_buffer_state bs = rlc->get_buffer_state();
    EXPECT_EQ(bs.pending_bytes, 0);
    EXPECT_FALSE(bs.hol_toa.has_value());
    EXPECT_EQ(tester->bsr.pending_bytes, expect_buffer_state); // pull_pdu does not push BSR to lower layer
    EXPECT_TRUE(tester->bsr.hol_toa.has_value());
    EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
    EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
    EXPECT_EQ(tester->bsr_count, n_bsr);

    return out_pdus;
  }

  /// \brief Obtains RLC AMD PDU segments from generated SDUs that are passed through an RLC AM entity
  /// \param[in] n_pdus Length of the out_pdus array
  /// \param[in] pdu_size Maximum size of each PDU that is read from RLC AM entity
  /// \param[in] n_sdus Number of SDUs to push into RLC AM entity
  /// \param[in] sdu_size Size of each SDU that is pushed into RLC AM entity
  /// \return A vector of resulting RLC AMD PDUs
  std::vector<std::vector<uint8_t>> tx_segmented_pdus(uint32_t n_pdus,
                                                      uint32_t pdu_size,
                                                      uint32_t n_sdus,
                                                      uint32_t sdu_size,
                                                      uint32_t expect_remaining_bytes = 0)
  {
    std::vector<std::vector<uint8_t>> out_pdus;
    out_pdus.reserve(n_pdus);
    uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    uint32_t header_so_size  = 2;

    uint32_t n_bsr = tester->bsr_count;

    // Precondition
    EXPECT_LT(pdu_size, sdu_size + header_min_size) << "PDU size fits whole SDU; PDUs won't be segmented";

    // Push "n_sdus" SDUs into RLC
    auto sdu_bufs = std::vector<byte_buffer>(n_sdus);
    auto t_start  = std::chrono::steady_clock::now();
    for (uint32_t i = 0; i < n_sdus; i++) {
      sdu_bufs[i] = test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, i, sdu_size, i);

      // write SDU into upper end
      rlc->handle_sdu(sdu_bufs[i].deep_copy().value(), false); // keep local copy for later comparison
    }
    auto t_end = std::chrono::steady_clock::now();
    pcell_worker.run_pending_tasks();
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    uint32_t full_header_size    = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    uint32_t data_pdu_size       = full_header_size + sdu_size;
    uint32_t expect_buffer_state = n_sdus * data_pdu_size;

    // Read "n_pdus" PDUs from RLC
    uint32_t sdu_idx = 0;
    uint32_t sdu_so  = 0;
    for (uint32_t i = 0; i < n_pdus && rlc->get_buffer_state().pending_bytes > 0; i++) {
      uint32_t             header_size = header_min_size + (sdu_so == 0 ? 0 : header_so_size);
      std::vector<uint8_t> seg_buf;
      seg_buf.resize(pdu_size);
      size_t seg_len = rlc->pull_pdu(seg_buf);
      seg_buf.resize(seg_len);
      out_pdus.push_back(std::move(seg_buf));

      // Check PDU size
      EXPECT_GT(out_pdus[i].size(), header_size);
      EXPECT_LE(out_pdus[i].size(), pdu_size);

      // Check PDU payload
      auto pdu_begin = out_pdus[i].begin() + header_size;
      auto pdu_end   = out_pdus[i].end();
      auto sdu_begin = sdu_bufs[sdu_idx].begin() + sdu_so;
      EXPECT_TRUE(std::equal(pdu_begin, pdu_end, sdu_begin))
          << "sdu_idx=" << sdu_idx << ", sdu_so=" << sdu_so << ", i=" << i << ", header_size=" << header_size;

      // Check remaining buffer state
      uint32_t rem_sdus      = n_sdus - sdu_idx - 1;
      uint32_t rem_seg_bytes = sdu_bufs[sdu_idx].length() - sdu_so - out_pdus[i].size() + header_size;
      uint32_t rem_seg_hdr   = rem_seg_bytes > 0 ? header_min_size + header_so_size : 0;

      rlc_buffer_state bs = rlc->get_buffer_state();
      EXPECT_EQ(bs.pending_bytes,
                rem_sdus * (sdu_size + header_min_size) + rem_seg_bytes + rem_seg_hdr); // actual buffer state changes
      if (bs.pending_bytes == 0) {
        EXPECT_FALSE(bs.hol_toa.has_value());
      } else {
        EXPECT_TRUE(bs.hol_toa.has_value());
        EXPECT_GT(bs.hol_toa.value(), t_start);
        EXPECT_LT(bs.hol_toa.value(), t_end);
      }
      EXPECT_EQ(tester->bsr.pending_bytes, expect_buffer_state); // pull_pdu does not push BSR to lower layer
      EXPECT_TRUE(tester->bsr.hol_toa.has_value());
      EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
      EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
      EXPECT_EQ(tester->bsr_count, n_bsr);

      // Check SI
      rlc_si_field si, si_expect;
      si = static_cast<rlc_si_field>((*out_pdus[i].begin() >> 4) & 0b11);
      if (sdu_so == 0) {
        // full_sdu or first segment
        if (rem_seg_bytes == 0) {
          si_expect = rlc_si_field::full_sdu;
        } else {
          si_expect = rlc_si_field::first_segment;
        }
      } else {
        // last segment or middle segment;
        if (rem_seg_bytes == 0) {
          si_expect = rlc_si_field::last_segment;
        } else {
          si_expect = rlc_si_field::middle_segment;
        }
      }
      EXPECT_EQ(si, si_expect);

      // Update payload offsets
      if (rem_seg_bytes == 0) {
        sdu_idx++;
        sdu_so = 0;
      } else {
        sdu_so += out_pdus[i].size() - header_size;
      }
    }
    rlc_buffer_state bs = rlc->get_buffer_state();
    EXPECT_EQ(bs.pending_bytes, expect_remaining_bytes);
    if (bs.pending_bytes == 0) {
      EXPECT_FALSE(bs.hol_toa.has_value());
    } else {
      EXPECT_TRUE(bs.hol_toa.has_value());
      EXPECT_GT(bs.hol_toa.value(), t_start);
      EXPECT_LT(bs.hol_toa.value(), t_end);
    }
    EXPECT_EQ(tester->bsr.pending_bytes, expect_buffer_state); // pull_pdu does not push BSR to lower layer
    EXPECT_TRUE(tester->bsr.hol_toa.has_value());
    EXPECT_GT(tester->bsr.hol_toa.value(), t_start);
    EXPECT_LT(tester->bsr.hol_toa.value(), t_end);
    EXPECT_EQ(tester->bsr_count, n_bsr);

    return out_pdus;
  }

  void tick()
  {
    timers.tick();
    pcell_worker.run_pending_tasks();
  }

  srslog::basic_logger&                         logger  = srslog::fetch_basic_logger("TEST", false);
  rlc_am_sn_size                                sn_size = GetParam();
  rlc_tx_am_config                              config;
  timer_manager                                 timers;
  manual_task_worker                            ue_worker{128};
  manual_task_worker                            pcell_worker{128};
  std::unique_ptr<rlc_tx_am_test_frame>         tester;
  null_rlc_pcap                                 pcap;
  std::unique_ptr<rlc_tx_am_entity>             rlc;
  std::unique_ptr<rlc_bearer_metrics_collector> metrics_coll;
};

TEST_P(rlc_tx_am_test, create_new_entity)
{
  rlc_buffer_state bs = rlc->get_buffer_state();
  EXPECT_EQ(bs.pending_bytes, 0);
  EXPECT_FALSE(bs.hol_toa.has_value());
  EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), 0);
  EXPECT_EQ(tester->desired_buf_size_list.size(), 0);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 0);
  EXPECT_EQ(tester->bsr.pending_bytes, 0);
  EXPECT_FALSE(tester->bsr.hol_toa.has_value());
  EXPECT_EQ(tester->bsr_count, 0);
}

TEST_P(rlc_tx_am_test, tx_without_segmentation)
{
  const uint32_t n_pdus = 5;

  tx_full_pdus(n_pdus, 4);
  tx_full_pdus(n_pdus, 5);
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

  tx_segmented_pdus(n_pdus, pdu_size, n_sdus, sdu_size);
  tx_segmented_pdus(n_pdus, pdu_size, n_sdus, sdu_size);
}

TEST_P(rlc_tx_am_test, tx_insufficient_space_new_sdu)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t short_size      = header_min_size;
  const uint32_t fit_size        = header_min_size + sdu_size;

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  rlc->handle_sdu(test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, 0, sdu_size, 0), false);
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;

  // short read - expect empty PDU
  pdu_buf.resize(short_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, 0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // fitting read
  pdu_buf.resize(fit_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, fit_size);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);
}

TEST_P(rlc_tx_am_test, tx_insufficient_space_continued_sdu)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t header_so_size  = 2;
  const uint32_t min_size_first  = header_min_size + 1;
  const uint32_t short_size      = header_min_size + header_so_size;
  const uint32_t min_size_seg    = header_min_size + header_so_size + 1;

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  rlc->handle_sdu(test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, 0, sdu_size, 0), false);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;

  // kick-off segmentation
  pdu_buf.resize(min_size_first);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, min_size_first);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, (sdu_size - 1) + header_min_size + header_so_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // short read - expect empty PDU
  pdu_buf.resize(short_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, 0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, (sdu_size - 1) + header_min_size + header_so_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // minimum-length read (middle segment)
  pdu_buf.resize(min_size_seg);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, min_size_seg);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, (sdu_size - 2) + header_min_size + header_so_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // remaining-length read (last segment)
  const uint32_t remaining = (sdu_size - 2) + header_min_size + header_so_size;
  pdu_buf.resize(remaining);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, remaining);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, 1);
}

TEST_P(rlc_tx_am_test, sdu_discard)
{
  const uint32_t sdu_size = 4;
  const uint32_t n_pdus   = 6;
  uint32_t       n_bsr    = tester->bsr_count;

  // Push "n_pdus" SDUs into RLC
  byte_buffer sdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    sdu_bufs[i] = test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, i, sdu_size, i);

    // write SDU into upper end
    rlc->handle_sdu(sdu_bufs[i].deep_copy().value(), false); // keep local copy for later comparison
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
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 0);

  // Try discard of invalid SDU
  rlc->discard_sdu(999);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 1);

  // Try discard of already discarded SDU
  rlc->discard_sdu(0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 2);

  // Transmit full PDU
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(data_pdu_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  EXPECT_GT(pdu_len, 0);
  EXPECT_TRUE(std::equal(pdu_buf.begin() + header_size, pdu_buf.end(), sdu_bufs[1].begin()));
  pcell_worker.run_pending_tasks();
  expect_buffer_state = (n_pdus - 4) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 2);

  // Try discard of already transmitted SDU
  rlc->discard_sdu(1);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 3);

  // Transmit full PDU
  pdu_buf.resize(data_pdu_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  EXPECT_GT(pdu_len, 0);
  EXPECT_TRUE(std::equal(pdu_buf.begin() + header_size, pdu_buf.end(), sdu_bufs[2].begin()));
  expect_buffer_state = (n_pdus - 5) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 3);

  // Discard remaining SDU
  rlc->discard_sdu(5);
  expect_buffer_state = 0;
  expect_mac_bsr      = 0;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 4);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 3);
}

TEST_P(rlc_tx_am_test, sdu_discard_with_pdcp_sn_wraparound)
{
  const uint32_t sdu_size = 4;
  const uint32_t n_pdus   = 6;
  uint32_t       n_bsr    = tester->bsr_count;

  const uint32_t pdcp_sn_mod   = config.pdcp_sn_len == pdcp_sn_size::size12bits ? 4096 : 262144;
  const uint32_t pdcp_sn_start = pdcp_sn_mod - 4;

  // Push "n_pdus" SDUs into RLC
  byte_buffer sdu_bufs[n_pdus];
  for (uint32_t i = 0; i < n_pdus; i++) {
    sdu_bufs[i] =
        test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, pdcp_sn_start + i, sdu_size, i);

    // write SDU into upper end
    rlc->handle_sdu(sdu_bufs[i].deep_copy().value(), false); // keep local copy for later comparison
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
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 0);

  // Try discard of invalid SDU
  rlc->discard_sdu((pdcp_sn_start + 999) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 1);

  // Try discard of already discarded SDU
  rlc->discard_sdu((pdcp_sn_start + 0) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 2);

  // Transmit full PDU
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(data_pdu_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  EXPECT_GT(pdu_len, 0);
  EXPECT_TRUE(std::equal(pdu_buf.begin() + header_size, pdu_buf.end(), sdu_bufs[1].begin()));
  expect_buffer_state = (n_pdus - 4) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 2);

  // Try discard of already transmitted SDU
  rlc->discard_sdu((pdcp_sn_start + 1) % pdcp_sn_mod);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 3);

  // Transmit full PDU
  pdu_buf.resize(data_pdu_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  EXPECT_GT(pdu_len, 0);
  EXPECT_TRUE(std::equal(pdu_buf.begin() + header_size, pdu_buf.end(), sdu_bufs[2].begin()));
  expect_buffer_state = (n_pdus - 5) * data_pdu_size;
  expect_mac_bsr      = (n_pdus - 3) * data_pdu_size;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 3);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 3);

  // Discard remaining SDU
  rlc->discard_sdu((pdcp_sn_start + 5) % pdcp_sn_mod);
  expect_buffer_state = 0;
  expect_mac_bsr      = 0;
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, expect_buffer_state);
  EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discarded_sdus, 4);
  EXPECT_EQ(rlc->get_metrics().tx_high.num_discard_failures, 3);
}

TEST_P(rlc_tx_am_test, invalid_status_report_ack_sn_larger_than_tx_next)
{
  const uint32_t sdu_size = 4;
  const uint32_t n_pdus   = 5;

  std::vector<std::vector<uint8_t>> pdus = tx_full_pdus(n_pdus, sdu_size);

  // NACK SN=3
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn       = n_pdus + 2;
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0); // invalid status report ignored

  // Verify protocol error notification
  ASSERT_EQ(tester->proto_err_count, 1);
}

TEST_P(rlc_tx_am_test, retx_pdu_without_segmentation)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx as full PDU
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(sdu_size + header_min_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_EQ(pdu_buf, pdus[nack.nack_sn]);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_with_segmentation)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t so_size         = 2;
  const uint32_t header_max_size = header_min_size + so_size;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx in segments
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  for (uint32_t i = 0; i < sdu_size; i++) {
    uint32_t header_size = i == 0 ? header_min_size : header_max_size;
    pdu_buf.resize(1 + header_size);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
    logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
    EXPECT_TRUE(
        std::equal(pdu_buf.begin() + header_size, pdu_buf.end(), pdus[nack.nack_sn].begin() + header_min_size + i));
    EXPECT_EQ(tester->bsr_count, n_bsr);

    // Check SI
    rlc_si_field si;
    rlc_si_field si_expect;
    si = static_cast<rlc_si_field>((*pdu_buf.begin() >> 4) & 0b11);
    if (i == 0) {
      // first segment
      si_expect = rlc_si_field::first_segment;
    } else if (i == sdu_size - 1) {
      // last segment
      si_expect = rlc_si_field::last_segment;
    } else {
      // middle segment
      si_expect = rlc_si_field::middle_segment;
    }
    EXPECT_EQ(si, si_expect);
  }
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), 5);
  ASSERT_EQ(tester->desired_buf_size_list.size(), 5);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_first_segment_without_segmentation)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=3 0:1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 0;
  nack.so_end             = 1;
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, nack.so_end - nack.so_start + 1 + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end - nack.so_start + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(sdu_size + header_min_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(
      std::equal(pdu_buf.begin() + header_min_size, pdu_buf.end(), pdus[nack.nack_sn].begin() + header_min_size));
  // Check SI
  EXPECT_EQ(static_cast<rlc_si_field>((*pdu_buf.begin() >> 4) & 0b11), rlc_si_field::first_segment);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end - nack.so_start + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_middle_segment_without_segmentation)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t so_size         = 2;
  const uint32_t header_max_size = header_min_size + so_size;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=3 1:1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 1;
  nack.so_end             = 1;
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(sdu_size + header_max_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(std::equal(
      pdu_buf.begin() + header_max_size, pdu_buf.end(), pdus[nack.nack_sn].begin() + header_min_size + nack.so_start));
  // Check SI
  EXPECT_EQ(static_cast<rlc_si_field>((*pdu_buf.begin() >> 4) & 0b11), rlc_si_field::middle_segment);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_last_segment_without_segmentation)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t so_size         = 2;
  const uint32_t header_max_size = header_min_size + so_size;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=3 1:3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 1;
  nack.so_end             = 3;
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(sdu_size + header_max_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(std::equal(
      pdu_buf.begin() + header_max_size, pdu_buf.end(), pdus[nack.nack_sn].begin() + header_min_size + nack.so_start));
  // Check SI
  EXPECT_EQ(static_cast<rlc_si_field>((*pdu_buf.begin() >> 4) & 0b11), rlc_si_field::last_segment);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end - nack.so_start + 1 + header_max_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_of_sn_under_segmentation_is_trimmed_to_already_sent_bytes)
{
  const uint32_t n_sdus   = 5;
  const uint32_t sdu_size = 9;

  const uint32_t n_splits = 3;

  const uint32_t n_pdus      = n_sdus * n_splits - 1;
  const uint32_t header_size = (sn_size == rlc_am_sn_size::size12bits ? 2 : 3);
  const uint32_t so_size     = 2;
  const uint32_t pdu_size    = header_size + so_size + (sdu_size / n_splits);

  const uint32_t unsent_sdu_bytes = sdu_size / n_splits - so_size; // subtract 2 extra bytes from 1st segment (no SO)
  tx_segmented_pdus(n_pdus, pdu_size, n_sdus, sdu_size, unsent_sdu_bytes + header_size + so_size);

  // The rest of the SDU under segmentation is waiting for transmission
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);

  // NACK SN=4 2:65535 => this SN is currently under segmentation; ReTx only the parts that haven't been sent yet.
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 4;
  nack.has_so             = true;
  nack.so_start           = so_size + (sdu_size / n_splits); // assume 1st segment is received properly
  nack.so_end             = nack.so_end_of_sdu;              // the rest is missing
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = n_sdus;
    status_pdu.push_nack(nack);
    rlc->on_status_pdu(std::move(status_pdu));
  }
  pcell_worker.run_pending_tasks();

  const uint32_t retx_bytes = sdu_size - nack.so_start - unsent_sdu_bytes;
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes,
            unsent_sdu_bytes + header_size + so_size + header_size + so_size + retx_bytes);

  // Read the ReTx segment which shall only include bytes that were already sent
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(header_size + so_size + retx_bytes);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  EXPECT_EQ(pdu_len, header_size + so_size + retx_bytes);
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);

  // Read the rest of the SDU under segmentation that was not yet sent
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);
  pdu_buf.resize(unsent_sdu_bytes + header_size + so_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
}

TEST_P(rlc_tx_am_test, retx_of_sn_under_segmentation_is_ignored_for_unsent_bytes)
{
  const uint32_t n_sdus   = 5;
  const uint32_t sdu_size = 9;

  const uint32_t n_splits = 3;

  const uint32_t n_pdus      = n_sdus * n_splits - 1;
  const uint32_t header_size = (sn_size == rlc_am_sn_size::size12bits ? 2 : 3);
  const uint32_t so_size     = 2;
  const uint32_t pdu_size    = header_size + so_size + (sdu_size / n_splits);

  const uint32_t unsent_sdu_bytes = sdu_size / n_splits - so_size; // subtract 2 extra bytes from 1st segment (no SO)
  tx_segmented_pdus(n_pdus, pdu_size, n_sdus, sdu_size, unsent_sdu_bytes + header_size + so_size);

  // The rest of the SDU under segmentation is waiting for transmission
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);

  // NACK SN=4 8:65535 => this SN is currently under segmentation; Ignore NACK of parts that haven't been sent yet.
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 4;
  nack.has_so             = true;
  nack.so_start           = sdu_size - unsent_sdu_bytes; // assume 1st segment is received properly
  nack.so_end             = nack.so_end_of_sdu;          // the rest is missing
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = n_sdus;
    status_pdu.push_nack(nack);
    rlc->on_status_pdu(std::move(status_pdu));
  }
  pcell_worker.run_pending_tasks();

  // Expect no ReTx was added
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);

  // NACK SN=4 99:65535 => Ignore NACK of out-of-bounds bytes of SDu under segmentation.
  nack          = {};
  nack.nack_sn  = 4;
  nack.has_so   = true;
  nack.so_start = 99;                 // out-of-bounds
  nack.so_end   = nack.so_end_of_sdu; // the rest is missing
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = n_sdus;
    status_pdu.push_nack(nack);
    rlc->on_status_pdu(std::move(status_pdu));
  }
  pcell_worker.run_pending_tasks();

  // Expect no ReTx was added
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);

  // Read the rest of the SDU under segmentation that was not yet sent
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);
  pdu_buf.resize(unsent_sdu_bytes + header_size + so_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
}

TEST_P(rlc_tx_am_test, retx_only_sent_bytes_of_sn_under_segmentation)
{
  const uint32_t n_sdus   = 5;
  const uint32_t sdu_size = 9;

  const uint32_t n_splits = 3;

  const uint32_t n_pdus      = n_sdus * n_splits - 1;
  const uint32_t header_size = (sn_size == rlc_am_sn_size::size12bits ? 2 : 3);
  const uint32_t so_size     = 2;
  const uint32_t pdu_size    = header_size + so_size + (sdu_size / n_splits);

  const uint32_t unsent_sdu_bytes = sdu_size / n_splits - so_size; // subtract 2 extra bytes from 1st segment (no SO)
  tx_segmented_pdus(n_pdus, pdu_size, n_sdus, sdu_size, unsent_sdu_bytes + header_size + so_size);

  // NACK SN=4 2:65535 => this SN is currently under segmentation; ReTx only the parts that haven't been sent yet.
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 4;
  nack.has_so             = true;
  nack.so_start           = so_size + (sdu_size / n_splits); // assume 1st segment is received properly
  nack.so_end             = nack.so_end_of_sdu;              // the rest is missing
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = n_sdus;
    status_pdu.push_nack(nack);
    rlc->on_status_pdu(std::move(status_pdu));
  }
  pcell_worker.run_pending_tasks();

  const uint32_t retx_bytes = sdu_size - nack.so_start - unsent_sdu_bytes;
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes,
            unsent_sdu_bytes + header_size + so_size + header_size + so_size + retx_bytes);

  // read the ReTx segment which shall only include bytes that were already sent
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(header_size + so_size + retx_bytes);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  EXPECT_EQ(pdu_len, header_size + so_size + retx_bytes);
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);

  // read the rest of the SDU under segmentation that was not yet sent
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, unsent_sdu_bytes + header_size + so_size);
  pdu_buf.resize(unsent_sdu_bytes + header_size + so_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
}

TEST_P(rlc_tx_am_test, retx_pdu_segment_invalid_so_start_and_so_end)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=3 4:4
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 4; // out of bounds
  nack.so_end             = 4; // out of bounds
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(sdu_size + header_min_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(
      std::equal(pdu_buf.begin() + header_min_size, pdu_buf.end(), pdus[nack.nack_sn].begin() + header_min_size));
  // Check SI
  EXPECT_EQ(static_cast<rlc_si_field>((*pdu_buf.begin() >> 4) & 0b11), rlc_si_field::full_sdu);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_segment_invalid_so_start_larger_than_so_end)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=3 3:2
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  nack.so_start           = 2; // larger than so_end
  nack.so_end             = 1; // smaller than so_start
  nack.has_so             = true;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, nack.so_end + 1 + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx without extra segmentation
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(sdu_size + header_min_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_TRUE(
      std::equal(pdu_buf.begin() + header_min_size, pdu_buf.end(), pdus[nack.nack_sn].begin() + header_min_size));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, nack.so_end + 1 + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, tx_huge_bursts_report_buffer_state_correctly)
{
  const uint32_t sdu_size        = 1500;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t num_sdus        = 2 * (MAX_DL_PDU_LENGTH / sdu_size);
  const uint32_t num_pdus        = num_sdus;

  // Push many SDUs into RLC above MAX_DL_PDU_LENGTH to limit excessive buffer state reports
  tx_full_pdus(num_pdus, sdu_size);

  // Queried buffer state should be up to date
  rlc_buffer_state bs1 = rlc->get_buffer_state();
  EXPECT_EQ(bs1.pending_bytes, 0);
  // Notified buffer state should reflect the filled RLC before pulling any PDUs
  EXPECT_EQ(tester->bsr.pending_bytes, num_pdus * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr_count, 1);

  // Push again many SDUs into RLC1 above MAX_DL_PDU_LENGTH; expect one new buffer status report
  tx_full_pdus(num_pdus, sdu_size);

  // Queried buffer state should be up to date
  bs1 = rlc->get_buffer_state();
  EXPECT_EQ(bs1.pending_bytes, 0);
  // Notified buffer state should reflect the re-filled RLC before pulling any PDUs
  EXPECT_EQ(tester->bsr.pending_bytes, num_pdus * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr_count, 2);
}

TEST_P(rlc_tx_am_test, retx_many_pdus_and_notify_mac)
{
  const uint32_t sdu_size        = 1500;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = MAX_DL_PDU_LENGTH / sdu_size + 1;

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK everything
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;

  for (uint32_t nack_sn = 0; nack_sn < n_pdus; nack_sn++) {
    rlc_am_status_nack nack = {};
    nack.nack_sn            = nack_sn;
    status_pdu.push_nack(nack);
  }
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, n_pdus * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr.pending_bytes, n_pdus * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs is zero
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 0);
}

TEST_P(rlc_tx_am_test, retx_hol_toa_has_priority)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;
  const uint32_t n_pdus          = 5;

  // Send first bunch of SDUs of which one will be ReTx'ed
  auto                              t_retx_start = std::chrono::steady_clock::now();
  std::vector<std::vector<uint8_t>> pdus         = tx_full_pdus(n_pdus, sdu_size);
  auto                              t_retx_end   = std::chrono::steady_clock::now();

  // Put another SDU in SDU queue
  auto t_tx_start = std::chrono::steady_clock::now();
  rlc->handle_sdu(test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, 0, sdu_size, 0), false);
  auto t_tx_end = std::chrono::steady_clock::now();
  pcell_worker.run_pending_tasks();
  rlc_buffer_state bs = rlc->get_buffer_state();
  EXPECT_EQ(bs.pending_bytes, pdu_size);
  EXPECT_TRUE(bs.hol_toa.has_value());
  EXPECT_GT(bs.hol_toa.value(), t_tx_start);
  EXPECT_LT(bs.hol_toa.value(), t_tx_end);
  EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr_count, 2);

  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=2
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 2;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_EQ(bs.pending_bytes, 2 * pdu_size);
  EXPECT_TRUE(bs.hol_toa.has_value());
  // TOA must be within ReTx interval
  EXPECT_GT(bs.hol_toa.value(), t_retx_start);
  EXPECT_LT(bs.hol_toa.value(), t_retx_end);
  // TOA must be smaller than Tx interval
  EXPECT_LT(bs.hol_toa.value(), t_tx_start);
  EXPECT_LT(bs.hol_toa.value(), t_tx_end);

  EXPECT_EQ(tester->bsr.pending_bytes, 2 * pdu_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx as full PDU
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(sdu_size + header_min_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  pdu_buf.resize(pdu_len);
  logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
  logger.debug(pdus[nack.nack_sn].begin(), pdus[nack.nack_sn].end(), "pdus[{}]:", nack.nack_sn);
  EXPECT_EQ(pdu_buf, pdus[nack.nack_sn]);
  pcell_worker.run_pending_tasks();
  bs = rlc->get_buffer_state();
  EXPECT_EQ(bs.pending_bytes, pdu_size);
  EXPECT_TRUE(bs.hol_toa.has_value());
  // TOA must be greater than ReTx interval
  EXPECT_GT(bs.hol_toa.value(), t_retx_start);
  EXPECT_GT(bs.hol_toa.value(), t_retx_end);
  // TOA must be within Tx interval
  EXPECT_GT(bs.hol_toa.value(), t_tx_start);
  EXPECT_LT(bs.hol_toa.value(), t_tx_end);

  EXPECT_EQ(tester->bsr.pending_bytes, 2 * pdu_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);
}

TEST_P(rlc_tx_am_test, invalid_nack_nack_sn_outside_rx_window)
{
  const uint32_t sdu_size = 4;
  const uint32_t n_pdus   = 12;

  std::vector<std::vector<uint8_t>> pdus = tx_full_pdus(n_pdus, sdu_size);

  // ACK up until 5
  rlc_am_status_pdu status_pdu0(sn_size);
  status_pdu0.ack_sn = 5;
  rlc->on_status_pdu(std::move(status_pdu0));

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
  rlc->on_status_pdu(std::move(status_pdu2));
  ASSERT_NE(rlc->get_buffer_state().pending_bytes, 0); // should have one PDU to RETX

  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_GT(pdu_len, 0);
  rlc_tx_am_state st0 = rlc->get_state();
  ASSERT_EQ(st0.tx_next_ack, 7); // TX_NEXT_ACK should be the same as NACK_SN now

  // Receive 1 now
  rlc->on_status_pdu(std::move(status_pdu1));          // NACK SN=6 is now outside TX window
  ASSERT_EQ(rlc->get_buffer_state().pending_bytes, 0); // should have ignored status report
  rlc_tx_am_state st1 = rlc->get_state();
  ASSERT_EQ(st1.tx_next_ack, 7); // TX_NEXT_ACK should have not changed
}

TEST_P(rlc_tx_am_test, invalid_nack_sn_larger_than_ack_sn)
{
  const uint32_t sdu_size = 4;
  const uint32_t n_pdus   = 12;

  std::vector<std::vector<uint8_t>> pdus = tx_full_pdus(n_pdus, sdu_size);

  // ACK up until 5
  rlc_am_status_pdu status_pdu0(sn_size);
  status_pdu0.ack_sn = 5;
  rlc->on_status_pdu(std::move(status_pdu0));

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
  rlc->on_status_pdu(std::move(status_pdu1));
  ASSERT_EQ(rlc->get_buffer_state().pending_bytes, 0); // should have ignored status report
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
  rlc->on_status_pdu(std::move(status_pdu2));
  ASSERT_NE(rlc->get_buffer_state().pending_bytes, 0); // should have processed status report

  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
  pdu_len = rlc->pull_pdu(pdu_buf); // RETX 5
  EXPECT_GT(pdu_len, 0);
  pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
  pdu_len = rlc->pull_pdu(pdu_buf); // RETX 6
  EXPECT_GT(pdu_len, 0);
  pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
  pdu_len = rlc->pull_pdu(pdu_buf); // RETX 7
  EXPECT_GT(pdu_len, 0);
  ASSERT_EQ(rlc->get_buffer_state().pending_bytes, 0);
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
  rlc->on_status_pdu(std::move(status_pdu3));
  ASSERT_EQ(rlc->get_buffer_state().pending_bytes, 0); // should have dropped status report
  rlc_tx_am_state st3 = rlc->get_state();
  ASSERT_EQ(st3.tx_next_ack, 5); // TX_NEXT_ACK should have not changed
}

TEST_P(rlc_tx_am_test, retx_insufficient_space)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t short_size      = header_min_size;
  const uint32_t n_pdus          = 5;

  std::vector<std::vector<uint8_t>> pdus = tx_full_pdus(n_pdus, sdu_size);
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  uint32_t n_bsr = tester->bsr_count;

  // NACK SN=3
  rlc_am_status_nack nack = {};
  nack.nack_sn            = 3;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = n_pdus;
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // short read of ReTx
  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;
  pdu_buf.resize(short_size);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, 0);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr.pending_bytes, sdu_size + header_min_size);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 2);
}

TEST_P(rlc_tx_am_test, retx_pdu_range_without_segmentation)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  uint32_t sn_start = cardinality(to_number(sn_size)) - 2;
  uint32_t nack_sn  = sn_start + 1;
  uint32_t range    = 3;

  // Configure state to upper edge
  rlc_tx_am_state st = {};
  st.tx_next         = sn_start;
  st.tx_next_ack     = sn_start;
  st.poll_sn         = sn_start;
  rlc->set_state(st);

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=nack_sn..nack_sn+range-1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = nack_sn;
  nack.has_nack_range     = true;
  nack.nack_range         = range;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = (sn_start + n_pdus) % cardinality(to_number(sn_size));
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr.pending_bytes, range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx as full PDU
  for (uint32_t i = nack_sn; i < nack_sn + range; i++) {
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(sdu_size + header_min_size);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
    logger.debug(pdus[i - sn_start].begin(), pdus[i - sn_start].end(), "pdus[{}]:", i - sn_start);
    EXPECT_TRUE(std::equal(pdu_buf.begin() + 1, pdu_buf.end(), pdus[i - sn_start].begin() + 1)); // skip header P bit
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, (nack_sn + range - 1 - i) * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr.pending_bytes, range * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 0);
}

TEST_P(rlc_tx_am_test, retx_pdu_range_wraparound)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  uint32_t sn_start = cardinality(to_number(sn_size)) - 2;
  uint32_t nack_sn  = sn_start + 1;
  uint32_t range    = 4; // maximum valid range is 4

  // Configure state to upper edge
  rlc_tx_am_state st = {};
  st.tx_next         = sn_start;
  st.tx_next_ack     = sn_start;
  st.poll_sn         = sn_start;
  rlc->set_state(st);

  std::vector<std::vector<uint8_t>> pdus  = tx_full_pdus(n_pdus, sdu_size);
  uint32_t                          n_bsr = tester->bsr_count;

  // NACK SN=nack_sn..nack_sn+range-1
  rlc_am_status_nack nack = {};
  nack.nack_sn            = nack_sn;
  nack.has_nack_range     = true;
  nack.nack_range         = range;
  rlc_am_status_pdu status_pdu(sn_size);
  status_pdu.ack_sn = (sn_start + n_pdus) % cardinality(to_number(sn_size));
  status_pdu.push_nack(nack);
  rlc->on_status_pdu(std::move(status_pdu));
  pcell_worker.run_pending_tasks();
  ue_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr.pending_bytes, range * (sdu_size + header_min_size));
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  // Read ReTx as full PDU
  for (uint32_t i = nack_sn; i < nack_sn + range; i++) {
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(sdu_size + header_min_size);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    logger.debug(pdu_buf.begin(), pdu_buf.end(), "retx_pdu:");
    logger.debug(pdus[i - sn_start].begin(), pdus[i - sn_start].end(), "pdus[{}]:", i - sn_start);
    ASSERT_EQ(pdu_buf.size(), pdus[i - sn_start].size());
    // compare PDUs, ignore P bit in first header byte
    EXPECT_EQ(*pdu_buf.begin() & 0x4, *pdus[i - sn_start].begin() & 0x4);                        // mask P bit
    EXPECT_TRUE(std::equal(pdu_buf.begin() + 1, pdu_buf.end(), pdus[i - sn_start].begin() + 1)); // check rest
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, (nack_sn + range - 1 - i) * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr.pending_bytes, range * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  // Verify transmit notification for queued SDUs
  ASSERT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), n_pdus);
  ASSERT_EQ(tester->desired_buf_size_list.size(), n_pdus);
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_pdus; pdcp_sn++) {
    EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.front(), pdcp_sn);
    EXPECT_EQ(tester->desired_buf_size_list.front(), config.queue_size_bytes);
    tester->highest_transmitted_pdcp_sn_list.pop_front();
    tester->desired_buf_size_list.pop_front();
  }

  // Verify delivery notification for fully ACK'ed SDUs
  ASSERT_EQ(tester->highest_delivered_pdcp_sn_list.size(), 1);
  EXPECT_EQ(tester->highest_delivered_pdcp_sn_list.front(), 0);
}

TEST_P(rlc_tx_am_test, buffer_state_considers_status_report)
{
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->highest_transmitted_pdcp_sn_list.size(), 0);
  EXPECT_EQ(tester->desired_buf_size_list.size(), 0);

  // First set the status_required flag without triggering the on_status_report_changed() event
  tester->status_required = true;
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr.pending_bytes, 0); // unchanged
  EXPECT_EQ(tester->bsr_count, 0);         // unchanged

  rlc_am_status_nack nack = {};
  nack.has_nack_range     = true;
  nack.has_so             = true;
  tester->status.push_nack(nack);
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr.pending_bytes, 0); // unchanged
  EXPECT_EQ(tester->bsr_count, 0);         // unchanged

  // Now trigger on_status_report_changed() as well and check for the expected outcomes
  rlc->on_status_report_changed();
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(tester->bsr.pending_bytes, tester->status.get_packed_size()); // expecting the size of the status report
  EXPECT_EQ(tester->bsr_count, 1);                                        // a bsr should be sent to lower layers

  // Now unflag status_required (RX entity does so when a status report was picked); this shouldn't trigger anything
  tester->status_required = false;
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  EXPECT_EQ(tester->bsr.pending_bytes, tester->status.get_packed_size()); // unchanged
  EXPECT_EQ(tester->bsr_count, 1);                                        // unchanged
}

TEST_P(rlc_tx_am_test, status_report_priority)
{
  // First push one SDU into RLC to also check precedence of status report
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
  rlc->handle_sdu(test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, 0, sdu_size, 0), false);
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr_count, 1);

  // Set status report required
  tester->status_required = true;
  rlc->on_status_report_changed();
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr_count, 2);

  tester->status.ack_sn   = 20;
  rlc_am_status_nack nack = {};

  nack.nack_sn = 5;
  tester->status.push_nack(nack);
  nack.nack_sn = 10;
  tester->status.push_nack(nack);
  nack.nack_sn = 15;
  tester->status.push_nack(nack);
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged

  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;

  // Read PDU, expect to be the status PDU
  pdu_buf.resize(tester->status.get_packed_size());
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, tester->status.get_packed_size());

  // The tester does not unflag status_required automatically, hence rlc still assumes that status PDU is needed
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, tester->status.get_packed_size() + pdu_size);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged

  // Disable status PDU requirement and check there is only the SDU waiting for Tx; this shouldn't trigger anything
  tester->status_required = false;
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr_count, 2); // unchanged
}

TEST_P(rlc_tx_am_test, status_report_trim)
{
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);

  // Set status report required
  tester->status_required = true;
  rlc->on_status_report_changed();
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr.pending_bytes, tester->status.get_packed_size());
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
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr.pending_bytes, tester->status.get_packed_size());
  EXPECT_EQ(tester->bsr_count, 2);

  std::vector<uint8_t> pdu_buf;
  size_t               pdu_len;

  // Fail to read status PDU due to insufficient grant size to fit a totally trimmed status PDU
  pdu_buf.resize(rlc_am_nr_status_pdu_sizeof_header_ack_sn - 1);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, 0);

  // Read trimmed status PDU (short by 1 byte, i.e. last NACK will be trimmed)
  pdu_buf.resize(tester->status.get_packed_size() - 1);
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, trimmed_size);

  // Read untrimmed status PDU
  pdu_buf.resize(tester->status.get_packed_size());
  pdu_len = rlc->pull_pdu(pdu_buf);
  EXPECT_EQ(pdu_len, tester->status.get_packed_size());
}

// Expired poll retransmit timer should trigger a ReTx if no fresh SDUs or ReTx are available.
// If the ReTx is transmitted without segmentation, the full PDU shall have the polling (P) bit set.
TEST_P(rlc_tx_am_test, expired_poll_retransmit_timer_triggers_retx_when_queues_are_empty_and_poll_in_full_pdu)
{
  const uint32_t n_pdus          = 5;
  const uint32_t sdu_size        = 10;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  tx_full_pdus(n_pdus, sdu_size);
  rlc_buffer_state expect_mac_bsr = tester->bsr;
  uint32_t         n_bsr          = tester->bsr_count;

  // advance timers to expire poll_retransmit_timer
  for (int i = 0; i < config.t_poll_retx; i++) {
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
    EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr.pending_bytes);
    EXPECT_EQ(tester->bsr_count, n_bsr);
    tick();
  }

  // expiry of poll_retransmit_timer should schedule an SDU for ReTx
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  {
    // pull one full PDU.
    // check if the polling (P) bit is set in the PDU header (because RLC queues become empty)
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_TRUE(pdu_hdr.p);
    EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }
}

// Expired poll retransmit timer should trigger a ReTx if no fresh SDUs or ReTx are available.
// If the ReTx is transmitted in segments, only the last segment shall have the polling (P) bit set.
TEST_P(rlc_tx_am_test, expired_poll_retransmit_timer_triggers_retx_when_queues_are_empty_and_poll_in_last_segment)
{
  const uint32_t n_pdus          = 5;
  const uint32_t sdu_size        = 10;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  tx_full_pdus(n_pdus, sdu_size);
  rlc_buffer_state expect_mac_bsr = tester->bsr;
  uint32_t         n_bsr          = tester->bsr_count;

  // advance timers to expire poll_retransmit_timer
  for (int i = 0; i < config.t_poll_retx; i++) {
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
    EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr.pending_bytes);
    EXPECT_EQ(tester->bsr_count, n_bsr);
    tick();
  }

  // expiry of poll_retransmit_timer should schedule an SDU for ReTx
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);

  {
    // pull one segment but leave 2 bytes for later.
    // check if the polling (P) bit is not set in the PDU header (queues are not empty yet)
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes - 2);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_FALSE(pdu_hdr.p);
    EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  {
    // pull next segment but leave 1 byte for later.
    // check if the polling (P) bit is not set in the PDU header (queues are not empty yet)
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes - 1);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_FALSE(pdu_hdr.p);
    EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }

  {
    // pull final segment so that RLC queues run empty.
    // check if the polling (P) bit is set in the PDU header (because RLC queues become empty)
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_TRUE(pdu_hdr.p);
    EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
  }
}

// Expired poll retransmit timer should not trigger an extra ReTx if fresh SDUs are available (and no stale window)
TEST_P(rlc_tx_am_test, expired_poll_retransmit_timer_triggers_no_retx_when_fresh_sdu_can_be_sent)
{
  const uint32_t n_pdus          = 5;
  const uint32_t sdu_size        = 10;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  tx_full_pdus(n_pdus, sdu_size);

  // push SDU to SDU queue so that it is not empty
  uint32_t    n_bsr   = tester->bsr_count;
  byte_buffer sdu_buf = test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, 7, sdu_size, 7);
  rlc->handle_sdu(sdu_buf.deep_copy().value(), false); // keep local copy for later comparison
  pcell_worker.run_pending_tasks();
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, pdu_size);
  EXPECT_EQ(tester->bsr_count, ++n_bsr);
  EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);

  {
    // pull one segment but leave 3 bytes for later.
    // check if the polling (P) bit is not set in the PDU header
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes - 3);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_FALSE(pdu_hdr.p);
  }

  // advance timers to expire poll_retransmit_timer
  uint32_t old_bsr = rlc->get_buffer_state().pending_bytes;
  for (int i = 0; i < config.t_poll_retx; i++) {
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, old_bsr);
    EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
    EXPECT_EQ(tester->bsr_count, n_bsr);
    tick();
  }

  // expiry of poll_retransmit_timer should not schedule any extra SDU for ReTx, since SDU queue is not empty
  EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
  EXPECT_EQ(rlc->get_buffer_state().pending_bytes, old_bsr);
  EXPECT_EQ(tester->bsr_count, n_bsr);

  {
    // pull next segment but leave 2 bytes for later.
    // check if the polling (P) bit is not set in the PDU header (queues are not empty yet)
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes - 2);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_FALSE(pdu_hdr.p);
  }

  {
    // pull next segment but leave 1 byte for later.
    // check if the polling (P) bit is not set in the PDU header (queues are not empty yet)
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes - 1);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_FALSE(pdu_hdr.p);
  }

  {
    // pull final segment so that RLC queues run empty.
    // check if the polling (P) bit is set in the PDU header (because RLC queues become empty)
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    rlc_am_pdu_header pdu_hdr = {};
    ASSERT_TRUE(
        rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
    EXPECT_TRUE(pdu_hdr.p);
  }
}

TEST_P(rlc_tx_am_test, expired_poll_retransmit_increments_retx_counter)
{
  const uint32_t n_pdus          = 5;
  const uint32_t sdu_size        = 10;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t pdu_size        = header_min_size + sdu_size;

  tx_full_pdus(n_pdus, sdu_size);

  for (uint32_t n_retx = 0; n_retx <= config.max_retx_thresh; n_retx++) {
    rlc_buffer_state expect_mac_bsr = tester->bsr;
    uint32_t         n_bsr          = tester->bsr_count;

    // no max_retx shall be reported yet
    EXPECT_EQ(tester->max_retx_count, 0);

    // advance timers to expire poll_retransmit_timer
    for (int32_t i = 0; i < config.t_poll_retx; i++) {
      EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
      EXPECT_EQ(tester->bsr.pending_bytes, expect_mac_bsr.pending_bytes) << "n_retx=" << n_retx << " i=" << i << "\n";
      EXPECT_EQ(tester->bsr_count, n_bsr);
      tick();
    }

    // expiry of poll_retransmit_timer should schedule an SDU for ReTx
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, pdu_size);
    EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    {
      // pull PDU
      // check if the polling (P) bit IS set in the PDU header (because of previously expired poll_retransmit_timer)
      std::vector<uint8_t> pdu_buf;
      size_t               pdu_len;
      pdu_buf.resize(rlc->get_buffer_state().pending_bytes);
      pdu_len = rlc->pull_pdu(pdu_buf);
      pdu_buf.resize(pdu_len);
      rlc_am_pdu_header pdu_hdr = {};
      ASSERT_TRUE(
          rlc_am_read_data_pdu_header(byte_buffer::create(pdu_buf.begin(), pdu_buf.end()).value(), sn_size, &pdu_hdr));
      EXPECT_TRUE(pdu_hdr.p);
      EXPECT_EQ(tester->bsr.pending_bytes, pdu_size);
      EXPECT_EQ(tester->bsr_count, n_bsr);
    }
  }

  // max_retx shall be reported now
  EXPECT_EQ(tester->max_retx_count, 1);
}

TEST_P(rlc_tx_am_test, retx_count_ignores_pending_retx)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  tx_full_pdus(n_pdus, sdu_size);
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
    rlc->on_status_pdu(std::move(status_pdu));
    pcell_worker.run_pending_tasks();
    ue_worker.run_pending_tasks();
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr.pending_bytes, 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    // Do not read both ReTx
  }

  // Expect that no max_retx has been reached
  EXPECT_EQ(tester->max_retx_count, 0);
}

TEST_P(rlc_tx_am_test, retx_count_trigger_max_retx_without_segmentation)
{
  const uint32_t sdu_size        = 4;
  const uint32_t header_min_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t n_pdus          = 5;

  tx_full_pdus(n_pdus, sdu_size);
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
    rlc->on_status_pdu(std::move(status_pdu));
    pcell_worker.run_pending_tasks();
    ue_worker.run_pending_tasks();
    EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr.pending_bytes, 2 * (sdu_size + header_min_size));
    EXPECT_EQ(tester->bsr_count, ++n_bsr);

    // Read both ReTx as full PDUs
    std::vector<uint8_t> pdu_buf;
    size_t               pdu_len;
    pdu_buf.resize(sdu_size + header_min_size);
    pdu_len = rlc->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    EXPECT_EQ(pdu_len, (sdu_size + header_min_size));

    if (n_retx != config.max_retx_thresh) {
      pdu_buf.resize(sdu_size + header_min_size);
      pdu_len = rlc->pull_pdu(pdu_buf);
      pdu_buf.resize(pdu_len);
      EXPECT_EQ(pdu_len, (sdu_size + header_min_size));

      EXPECT_EQ(rlc->get_buffer_state().pending_bytes, 0);
    } else {
      // max_retx already reached, last PDU is not read.
      pdu_buf.resize(sdu_size + header_min_size);
      pdu_len = rlc->pull_pdu(pdu_buf);
      EXPECT_EQ(pdu_len, 0);
    }
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
  fmt::format_to(std::back_inserter(buffer), "{}bit", to_number(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(rlc_tx_am_test_each_sn_size,
                         rlc_tx_am_test,
                         ::testing::Values(rlc_am_sn_size::size12bits, rlc_am_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
