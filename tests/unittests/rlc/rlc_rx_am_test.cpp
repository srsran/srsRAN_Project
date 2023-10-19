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

#include "lib/rlc/rlc_rx_am_entity.h"
#include "rlc_test_helpers.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <list>
#include <queue>
#include <utility>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC AM Rx entity.
class rlc_rx_am_test_frame : public rlc_rx_upper_layer_data_notifier,
                             public rlc_tx_am_status_handler,
                             public rlc_tx_am_status_notifier
{
public:
  std::queue<byte_buffer_chain> sdu_queue;
  uint32_t                      sdu_counter = 0;
  rlc_am_sn_size                sn_size;
  rlc_am_status_pdu             status;
  uint32_t                      status_trigger_counter = 0;

  rlc_rx_am_test_frame(rlc_am_sn_size sn_size_) : sn_size(sn_size_), status(sn_size_) {}

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }

  // rlc_tx_am_status_handler interface
  virtual void on_status_pdu(rlc_am_status_pdu status_) override { this->status = status_; }
  // rlc_tx_am_status_notifier interface
  virtual void on_status_report_changed() override { this->status_trigger_counter++; }
};

/// Fixture class for RLC AM Rx tests.
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class rlc_rx_am_test : public ::testing::Test, public ::testing::WithParamInterface<rlc_am_sn_size>, public rlc_trx_test
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

    logger.info("Creating RLC Rx AM entity ({} bit)", to_number(sn_size));

    // Set Rx config
    config.sn_field_length   = sn_size;
    config.t_reassembly      = 35;
    config.t_status_prohibit = 8;

    // Create test frame
    tester = std::make_unique<rlc_rx_am_test_frame>(config.sn_field_length);

    // Create RLC AM RX entity
    rlc = std::make_unique<rlc_rx_am_entity>(
        du_ue_index_t::MIN_DU_UE_INDEX, srb_id_t::srb0, config, *tester, timer_factory{timers, ue_worker}, ue_worker);

    // Bind AM Tx/Rx interconnect
    rlc->set_status_handler(tester.get());
    rlc->set_status_notifier(tester.get());
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Creates a list of RLC AMD PDU(s) containing either one RLC SDU or multiple RLC SDU segments
  ///
  /// The associated SDU contains an incremental sequence of bytes starting with the value given by first_byte,
  /// i.e. if first_byte = 0xfc and no segmentation, the PDU will be <header> 0xfc 0xfe 0xff 0x00 0x01 ...
  ///
  /// Note: Segmentation is applied if segment_size < sdu_size; Otherwise only one PDU with full SDU is produced,
  /// and the resulting list will hold only one PDU
  ///
  /// \param[out] pdu_list Reference to a list<byte_buffer> that is filled with the produced PDU(s)
  /// \param[out] sdu Reference to a byte_buffer that is filled with the associated SDU
  /// \param[in] sn The sequence number to be put in the PDU header
  /// \param[in] sdu_size Size of the SDU
  /// \param[in] segment_size Maximum payload size of each SDU or SDU segment.
  /// \param[in] first_byte Value of the first SDU payload byte
  void create_pdus(std::list<byte_buffer>& pdu_list,
                   byte_buffer&            sdu,
                   uint32_t                sn,
                   uint32_t                sdu_size,
                   uint32_t                segment_size,
                   uint8_t                 first_byte = 0) const
  {
    ASSERT_GT(sdu_size, 0) << "Invalid argument: Cannot create PDUs with zero-sized SDU";
    ASSERT_GT(segment_size, 0) << "Invalid argument: Cannot create PDUs with zero-sized SDU segments";

    sdu = create_sdu(sdu_size, first_byte);
    pdu_list.clear();
    byte_buffer_view rest = {sdu};

    rlc_am_pdu_header hdr = {};
    hdr.dc                = rlc_dc_field::data;
    hdr.p                 = 0;
    hdr.si                = rlc_si_field::full_sdu;
    hdr.sn_size           = config.sn_field_length;
    hdr.sn                = sn;
    hdr.so                = 0;
    do {
      byte_buffer_view payload = {};
      if (rest.length() > segment_size) {
        // first or middle segment
        if (hdr.so == 0) {
          hdr.si = rlc_si_field::first_segment;
        } else {
          hdr.si = rlc_si_field::middle_segment;
        }

        // split into payload and rest
        std::pair<byte_buffer_view, byte_buffer_view> split = rest.split(segment_size);

        payload = std::move(split.first);
        rest    = std::move(split.second);
      } else {
        // last segment or full PDU
        if (hdr.so == 0) {
          hdr.si = rlc_si_field::full_sdu;
        } else {
          hdr.si = rlc_si_field::last_segment;
        }

        // full payload, no rest
        payload = std::move(rest);
        rest    = {};
      }
      byte_buffer pdu_buf;
      logger.debug("AMD PDU header: {}", hdr);
      ASSERT_TRUE(rlc_am_write_data_pdu_header(hdr, pdu_buf));
      pdu_buf.append(payload);
      pdu_list.push_back(std::move(pdu_buf));

      // update segment offset for next iteration
      hdr.so += payload.length();
    } while (rest.length() > 0);
  }

  /// \brief Injects RLC AMD PDUs with full SDUs into the RLC AM entity starting from Sequence number sn_state
  /// \param[inout] sn_state Reference to the sequence number for the first SDU. Will be incremented for each SDU
  /// \param[in] n_sdus Number of SDUs
  /// \param[in] sdu_size SDU payload size
  /// \param[in] reverse_sdus Inject PDUs in reverse SDU order
  void rx_full_sdus(uint32_t& sn_state, uint32_t n_sdus, uint32_t sdu_size = 1, bool reverse_sdus = false)
  {
    // check status report
    rlc_am_status_pdu status_report = rlc->get_status_pdu();
    EXPECT_EQ(status_report.ack_sn, sn_state);
    EXPECT_EQ(status_report.get_nacks().size(), 0);
    EXPECT_EQ(status_report.get_packed_size(), 3);
    EXPECT_EQ(rlc->get_status_pdu_length(), 3);

    uint32_t expected_sn_state = sn_state;

    // Create SDUs and PDUs with full SDUs
    std::list<byte_buffer> pdu_originals = {};
    std::list<byte_buffer> sdu_originals = {};
    for (uint32_t i = 0; i < n_sdus; i++) {
      std::list<byte_buffer> pdu_list = {};
      byte_buffer            sdu;
      ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, sdu_size, sn_state));
      sn_state++;

      // save original PDU
      pdu_originals.push_back(std::move(pdu_list.front()));

      // save original SDU
      sdu_originals.push_back(std::move(sdu));
    }

    if (reverse_sdus) {
      pdu_originals.reverse();
      sdu_originals.reverse();
    }

    // Push PDUs into RLC
    for (byte_buffer& pdu_buf : pdu_originals) {
      // check status report
      status_report = rlc->get_status_pdu();
      EXPECT_EQ(status_report.ack_sn, expected_sn_state);
      EXPECT_EQ(status_report.get_nacks().size(), 0);
      EXPECT_EQ(status_report.get_packed_size(), 3);
      EXPECT_EQ(rlc->get_status_pdu_length(), 3);

      // write PDU into lower end
      byte_buffer_slice pdu = {std::move(pdu_buf)};
      rlc->handle_pdu(std::move(pdu));

      if (not reverse_sdus) {
        // According to 5.2.3.2.3, when transmitting in order:
        // st.rx_highest_status is advanced on each fully-received SDU.
        ++expected_sn_state;
      }
    }

    if (reverse_sdus) {
      // According to 5.2.3.2.3, when transmitting in reverse order:
      // st.rx_highest_status is only advanced after SDU with SN = (previous) st.rx_highest_status is fully received.
      expected_sn_state = sn_state;
    }
    // check status report
    status_report = rlc->get_status_pdu();
    EXPECT_EQ(status_report.ack_sn, expected_sn_state);
    EXPECT_EQ(status_report.get_nacks().size(), 0);
    EXPECT_EQ(status_report.get_packed_size(), 3);
    EXPECT_EQ(rlc->get_status_pdu_length(), 3);

    // Read "n_pdus" SDUs from upper layer
    ASSERT_EQ(tester->sdu_queue.size(), n_sdus);
    for (uint32_t i = 0; i < n_sdus; i++) {
      EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
      EXPECT_EQ(tester->sdu_queue.front(), sdu_originals.front());
      tester->sdu_queue.pop();
      sdu_originals.pop_front();
    }
    EXPECT_EQ(tester->sdu_queue.size(), 0);
    EXPECT_EQ(tester->status_trigger_counter, 0);
  }

  /// \brief Injects RLC AMD PDUs with SDU segments into the RLC AM entity starting from Sequence number sn_state
  /// \param[inout] sn_state Reference to the sequence number for the first SDU. Will be incremented for each SDU
  /// \param[in] n_sdus Number of SDUs
  /// \param[in] sdu_size SDU payload size
  /// \param[in] segment_size Maximum size of each SDU segment
  /// \param[in] reverse_sdus Reverse SDU order of injected PDUs
  /// \param[in] reverse_segments Reverse segment order of injected PDUs
  void rx_sdu_segments(uint32_t& sn_state,
                       uint32_t  n_sdus,
                       uint32_t  sdu_size         = 3,
                       uint32_t  segment_size     = 1,
                       bool      reverse_sdus     = false,
                       bool      reverse_segments = false)
  {
    // check status report
    rlc_am_status_pdu status_report = rlc->get_status_pdu();
    EXPECT_EQ(status_report.ack_sn, sn_state);
    EXPECT_EQ(status_report.get_nacks().size(), 0);
    EXPECT_EQ(status_report.get_packed_size(), 3);
    EXPECT_EQ(rlc->get_status_pdu_length(), 3);

    uint32_t expected_sn_state = sn_state;

    // Create SDUs and PDUs
    std::list<std::list<byte_buffer>> pdu_originals = {};
    std::list<byte_buffer>            sdu_originals = {};

    // Create SDUs and PDUs with SDU segments
    for (uint32_t i = 0; i < n_sdus; i++) {
      std::list<byte_buffer> pdu_list = {};
      byte_buffer            sdu;
      ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
      sn_state++;

      // save original PDUs
      pdu_originals.push_back(std::move(pdu_list));

      // save original SDU
      sdu_originals.push_back(std::move(sdu));

      if (reverse_segments) {
        pdu_originals.back().reverse();
      }
    }

    if (reverse_sdus) {
      pdu_originals.reverse();
      sdu_originals.reverse();
    }

    // Push PDUs into RLC
    for (std::list<byte_buffer>& segment_list : pdu_originals) {
      for (byte_buffer& pdu_buf : segment_list) {
        // check status report
        status_report = rlc->get_status_pdu();
        EXPECT_EQ(status_report.ack_sn, expected_sn_state);
        EXPECT_EQ(status_report.get_nacks().size(), 0);
        EXPECT_EQ(status_report.get_packed_size(), 3);
        EXPECT_EQ(rlc->get_status_pdu_length(), 3);

        byte_buffer_slice pdu = {std::move(pdu_buf)};
        rlc->handle_pdu(std::move(pdu));
      }

      if (not reverse_sdus) {
        // According to 5.2.3.2.3, when transmitting in order:
        // st.rx_highest_status is advanced on each fully-received SDU.
        ++expected_sn_state;
      }
    }

    if (reverse_sdus) {
      // According to 5.2.3.2.3, when transmitting in reverse order:
      // st.rx_highest_status is only advanced after SDU with SN = (previous) st.rx_highest_status is fully received.
      expected_sn_state = sn_state;
    }
    // check status report
    status_report = rlc->get_status_pdu();
    EXPECT_EQ(status_report.ack_sn, expected_sn_state);
    EXPECT_EQ(status_report.get_nacks().size(), 0);
    EXPECT_EQ(status_report.get_packed_size(), 3);
    EXPECT_EQ(rlc->get_status_pdu_length(), 3);

    // Read "n_sdus" SDUs from upper layer
    ASSERT_EQ(tester->sdu_queue.size(), n_sdus);
    for (uint32_t i = 0; i < n_sdus; i++) {
      EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
      EXPECT_EQ(tester->sdu_queue.front(), sdu_originals.front());
      tester->sdu_queue.pop();
      sdu_originals.pop_front();
    }
    EXPECT_EQ(tester->sdu_queue.size(), 0);
    EXPECT_EQ(tester->status_trigger_counter, 0);
  }

  void rx_overlapping_sdu_segments(uint32_t& sn_state,
                                   uint32_t  sdu_size       = 12,
                                   uint32_t  segment_size_a = 3,
                                   uint32_t  segment_size_b = 4,
                                   uint32_t  skip_a1        = 0,
                                   uint32_t  skip_a2        = 0)
  {
    // check status report
    rlc_am_status_pdu status_report = rlc->get_status_pdu();
    EXPECT_EQ(status_report.ack_sn, sn_state);
    EXPECT_EQ(status_report.get_nacks().size(), 0);
    EXPECT_EQ(status_report.get_packed_size(), 3);
    EXPECT_EQ(rlc->get_status_pdu_length(), 3);

    // Create SDU and PDUs with SDU segments
    std::list<byte_buffer> pdu_list_a = {};
    std::list<byte_buffer> pdu_list_b = {};
    byte_buffer            sdu;
    ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list_a, sdu, sn_state, sdu_size, segment_size_a, sn_state));
    ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list_b, sdu, sn_state, sdu_size, segment_size_b, sn_state));
    sn_state++;

    // Push A PDUs except for one into RLC
    uint32_t i = 0;
    for (const byte_buffer& pdu_buf : pdu_list_a) {
      if (i != skip_a1 && i != skip_a2) {
        byte_buffer_slice pdu = {pdu_buf.deep_copy()};
        rlc->handle_pdu(std::move(pdu));
      }
      i++;
    }

    // Check that nothing was forwarded to upper layer
    EXPECT_EQ(tester->sdu_queue.size(), 0);

    // Push all PDUs again; check that nothing is forwarded to upper layer before except after Rx of 5th segment
    // Push B PDUs into RLC
    for (const byte_buffer& pdu_buf : pdu_list_b) {
      byte_buffer_slice pdu = {pdu_buf.deep_copy()};
      rlc->handle_pdu(std::move(pdu));
    }
    ASSERT_EQ(tester->sdu_queue.size(), 1);
    EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
    EXPECT_EQ(tester->sdu_queue.front(), sdu);
    tester->sdu_queue.pop();
  }

  void tick()
  {
    timers.tick();
    ue_worker.run_pending_tasks();
  }

  srslog::basic_logger&                 logger  = srslog::fetch_basic_logger("TEST", false);
  rlc_am_sn_size                        sn_size = GetParam();
  rlc_rx_am_config                      config;
  timer_manager                         timers;
  manual_task_worker                    ue_worker{128};
  std::unique_ptr<rlc_rx_am_test_frame> tester;
  std::unique_ptr<rlc_rx_am_entity>     rlc;
};

/// Test the instantiation of a new entity
TEST_P(rlc_rx_am_test, create_new_entity)
{
  EXPECT_NE(rlc, nullptr);
}

/// Verify the status report from a freshly created instance
TEST_P(rlc_rx_am_test, read_initial_status)
{
  EXPECT_FALSE(rlc->status_report_required());
  rlc_am_status_pdu status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, 0);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);

  EXPECT_EQ(tester->status_trigger_counter, 0);
}

/// Verify Rx window boundary checks if Rx window is currently at the lower edge, i.e. at smallest SN value
TEST_P(rlc_rx_am_test, window_checker_lower_edge)
{
  // RX_NEXT == 0
  uint32_t sn_inside_lower  = 0;
  uint32_t sn_inside_upper  = window_size(to_number(sn_size)) - 1;
  uint32_t sn_outside_lower = cardinality(to_number(sn_size)) - 1;
  uint32_t sn_outside_upper = window_size(to_number(sn_size));
  EXPECT_TRUE(rlc->inside_rx_window(sn_inside_lower));
  EXPECT_TRUE(rlc->inside_rx_window(sn_inside_upper));
  EXPECT_FALSE(rlc->inside_rx_window(sn_outside_lower));
  EXPECT_FALSE(rlc->inside_rx_window(sn_outside_upper));
}

/// Verify Rx window boundary checks if Rx window is currently at the upper edge, i.e. at maximum SN value
TEST_P(rlc_rx_am_test, window_checker_upper_edge)
{
  // Configure state to upper edge
  rlc_rx_am_state st = {};
  st.rx_next         = cardinality(to_number(sn_size)) - 1;
  rlc->set_state(st);

  // RX_NEXT == 4095 (12-bit SN)
  uint32_t sn_inside_lower  = cardinality(to_number(sn_size)) - 1;
  uint32_t sn_inside_upper  = window_size(to_number(sn_size)) - 2;
  uint32_t sn_outside_lower = cardinality(to_number(sn_size)) - 2;
  uint32_t sn_outside_upper = window_size(to_number(sn_size)) - 1;
  EXPECT_TRUE(rlc->inside_rx_window(sn_inside_lower));
  EXPECT_TRUE(rlc->inside_rx_window(sn_inside_upper));
  EXPECT_FALSE(rlc->inside_rx_window(sn_outside_lower));
  EXPECT_FALSE(rlc->inside_rx_window(sn_outside_upper));
}

/// Verify proper forwarding of received status PDUs to the Tx entity (via interface)
TEST_P(rlc_rx_am_test, rx_valid_control_pdu)
{
  EXPECT_EQ(tester->status.ack_sn, INVALID_RLC_SN);
  EXPECT_EQ(tester->status.get_nacks().size(), 0);

  // Create status PDU with one NACK
  rlc_am_status_pdu status = {sn_size};
  status.ack_sn            = 1234;
  rlc_am_status_nack nack  = {};
  nack.nack_sn             = 1230;
  status.push_nack(nack);
  byte_buffer pdu_buf = {};
  EXPECT_TRUE(status.pack(pdu_buf));

  // Pass through RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc->handle_pdu(std::move(pdu));

  // Pick and verify the received status PDU on the other end
  EXPECT_EQ(tester->status.ack_sn, status.ack_sn);
  ASSERT_EQ(tester->status.get_nacks().size(), status.get_nacks().size());
  EXPECT_EQ(tester->status.get_nacks().front().nack_sn, nack.nack_sn);

  EXPECT_EQ(tester->status_trigger_counter, 0);
}

/// Verify that malformed status PDUs are not forwarded to the Tx entity (via interface)
TEST_P(rlc_rx_am_test, rx_invalid_control_pdu)
{
  EXPECT_EQ(tester->status.ack_sn, INVALID_RLC_SN);
  EXPECT_EQ(tester->status.get_nacks().size(), 0);

  // Create status PDU
  rlc_am_status_pdu status = {sn_size};
  status.ack_sn            = 1234;
  byte_buffer pdu_buf      = {};
  EXPECT_TRUE(status.pack(pdu_buf));

  // set reserved bits in CPT field
  *(pdu_buf.begin()) |= 0x70;

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc->handle_pdu(std::move(pdu));

  // Pick and verify the received status PDU on the other end
  EXPECT_EQ(tester->status.ack_sn, INVALID_RLC_SN);
  EXPECT_EQ(tester->status.get_nacks().size(), 0);
}

/// Verify empty PDUs are discarded.
TEST_P(rlc_rx_am_test, rx_empty_pdu)
{
  // Create empty PDU
  byte_buffer pdu_buf = {};

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc->handle_pdu(std::move(pdu));

  // Check if polling bit of malformed PDU was properly ignored
  EXPECT_EQ(tester->status_trigger_counter, 0);
}

/// Verify malformed (too short) data PDUs are discarded. Testing here with polling bit set in the malformed PDU which
/// shall not have any effect on the status-required state of the testee.
TEST_P(rlc_rx_am_test, rx_data_pdu_with_short_header)
{
  EXPECT_FALSE(rlc->status_report_required());

  // Create a short header of a data PDU with polling bit set
  byte_buffer pdu_buf = {};
  pdu_buf.append(0b11000000); // D/C = 1; P = 1

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc->handle_pdu(std::move(pdu));

  // Check if polling bit of malformed PDU was properly ignored
  EXPECT_FALSE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 0);
}

/// Verify malformed (too short) data PDUs are discarded. Testing here with polling bit set in the malformed PDU which
/// shall not have any effect on the status-required state of the testee.
TEST_P(rlc_rx_am_test, rx_data_pdu_without_payload)
{
  EXPECT_FALSE(rlc->status_report_required());

  // Create a complete header of a data PDU with polling bit set and with SO
  byte_buffer pdu_buf = {};
  pdu_buf.append(0b11110000); // D/C = 1; P = 1; SI = 0b11
  pdu_buf.append({0x00, 0x00, 0x00});
  if (sn_size == rlc_am_sn_size::size18bits) {
    pdu_buf.append(0x00);
  }

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc->handle_pdu(std::move(pdu));

  // Check if polling bit of malformed PDU was properly ignored
  EXPECT_FALSE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 0);
}

/// Verify proper handling of polling bit for valid PDUs inside the Rx window: The status-required state shall change
/// and the included SDU shall be forwarded to upper layer
TEST_P(rlc_rx_am_test, rx_polling_bit_sn_inside_rx_window)
{
  EXPECT_FALSE(rlc->status_report_required());

  uint32_t sn_state = 0;
  uint32_t sdu_size = 1;

  // Create SDU and PDU with full SDU
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, sdu_size, sn_state));
  sn_state++;

  // Change polling bit in first byte of PDU (header)
  *(pdu_list.front().begin()) |= 0b01000000; // set P = 1;

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_list.front())};
  rlc->handle_pdu(std::move(pdu));

  // Check if polling bit of PDU was properly considered
  EXPECT_TRUE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // Check if SDU was properly unpacked and forwarded
  ASSERT_EQ(tester->sdu_queue.size(), 1);
  EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
  EXPECT_EQ(tester->sdu_queue.front(), sdu);
  tester->sdu_queue.pop();
}

/// Verify proper handling of polling bit for PDUs outside the Rx window: The status-required state shall still change
/// but the included SDU shall be discarded
TEST_P(rlc_rx_am_test, rx_polling_bit_sn_outside_rx_window)
{
  EXPECT_FALSE(rlc->status_report_required());

  uint32_t sn_state = window_size(to_number(sn_size)); // out-of-window SN
  uint32_t sdu_size = 1;

  // Create SDU and PDU with full SDU
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, sdu_size, sn_state));
  sn_state++;

  // Change polling bit in first byte of PDU (header)
  *(pdu_list.front().begin()) |= 0b01000000; // set P = 1;

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_list.front())};
  rlc->handle_pdu(std::move(pdu));

  // Check if polling bit was considered, despite out-of-window SN
  EXPECT_TRUE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // Check if SDU was properly ignored
  ASSERT_EQ(tester->sdu_queue.size(), 0);
}

/// Verify proper handling of polling bit for PDU duplicates inside the Rx window: The status-required state shall still
/// change but the duplicated SDU shall be discarded
TEST_P(rlc_rx_am_test, rx_polling_bit_sdu_duplicate)
{
  EXPECT_FALSE(rlc->status_report_required());

  uint32_t sn_state = 1; // further incremented SN to prevent that reception will advance rx_window
  uint32_t sdu_size = 1;

  // Create SDU and PDU with full SDU
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, sdu_size, sn_state));
  sn_state++;

  // Push into RLC
  byte_buffer_slice pdu = {pdu_list.front().deep_copy()};
  rlc->handle_pdu(std::move(pdu));

  // Check if polling bit has not changed
  EXPECT_FALSE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 0);

  // Check if SDU was properly unpacked and forwarded
  ASSERT_EQ(tester->sdu_queue.size(), 1);
  EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
  EXPECT_EQ(tester->sdu_queue.front(), sdu);
  tester->sdu_queue.pop();

  // Change polling bit in first byte of PDU (header)
  *(pdu_list.front().begin()) |= 0b01000000; // set P = 1;

  // Push into RLC
  pdu = {pdu_list.front().deep_copy()};
  rlc->handle_pdu(std::move(pdu));

  // Check if polling bit was considered, despite duplicate SN
  EXPECT_TRUE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // Check if duplicate SDU was properly ignored
  ASSERT_EQ(tester->sdu_queue.size(), 0);
}

/// Verify handling of PDUs with SDU segment duplicates:
/// - Receive SDU in segments, but loose one segment.
/// - Receive all segments again, without any further loss:
///   - Duplicates shall be discarded
///   - SDU shall be properly reassembled and forwarded to upper layer as the missing segment is received
///   - Subsequent duplicates shall be discarded
TEST_P(rlc_rx_am_test, rx_duplicate_segments)
{
  uint32_t sn_state     = 0;
  uint32_t sdu_size     = 10;
  uint32_t segment_size = 1;

  // check status report
  rlc_am_status_pdu status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, sn_state);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);

  // Create SDU and PDUs with SDU segments
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
  sn_state++;

  // Push PDUs except for 5th into RLC
  int i = 0;
  for (const byte_buffer& pdu_buf : pdu_list) {
    if (i != 5) {
      byte_buffer_slice pdu = {pdu_buf.deep_copy()};
      rlc->handle_pdu(std::move(pdu));
    }
    i++;
  }

  // Check that nothing was forwarded to upper layer
  EXPECT_EQ(tester->sdu_queue.size(), 0);

  // Push all PDUs again; check that nothing is forwarded to upper layer before except after Rx of 5th segment
  i = 0;
  for (const byte_buffer& pdu_buf : pdu_list) {
    byte_buffer_slice pdu = {pdu_buf.deep_copy()};
    rlc->handle_pdu(std::move(pdu));
    if (i == 5) {
      // check if SDU has been assembled correctly
      ASSERT_EQ(tester->sdu_queue.size(), 1);
      EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
      EXPECT_EQ(tester->sdu_queue.front(), sdu);
      tester->sdu_queue.pop();
    } else {
      EXPECT_EQ(tester->sdu_queue.size(), 0);
    }
    i++;
  }
}

/// Verify that reception of fully overlapping segments is properly managed, i.e. segments get trimmed or are discarded
/// as required.
/// This test received a 8-byte SDU in 2-byte segments, leaving out one of the segments. Then the same SDU
/// is received in 4-byte segments. The content of the final SDU is checked against the original. The test is repeated
/// for all positions of the missing element.
TEST_P(rlc_rx_am_test, rx_partially_overlapping_segments_2_4)
{
  uint32_t sn_state       = 0;
  uint32_t sdu_size       = 8;
  uint32_t segment_size_a = 2;
  uint32_t segment_size_b = 4;

  // One segment missing
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 0, 0);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 1, 1);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 2, 2);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 3, 3);
  // Two segments missing
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 0, 1);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 0, 2);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 0, 3);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 1, 2);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 1, 3);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 2, 3);
}

/// Verify that reception of fully overlapping segments is properly managed, i.e. segments get trimmed or are discarded
/// as required.
/// This test received a 8-byte SDU in 4-byte segments, leaving out one of the segments.
/// Then the same SDU is received in 2-byte segments.
/// The content of the final SDU is checked against the original.
/// The test is repeated for all positions of the missing element.
TEST_P(rlc_rx_am_test, rx_partially_overlapping_segments_4_2)
{
  uint32_t sn_state       = 0;
  uint32_t sdu_size       = 8;
  uint32_t segment_size_a = 4;
  uint32_t segment_size_b = 2;

  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 0, 0);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 1, 1);
}

/// Verify that reception of partially overlapping segments is properly managed, i.e. segments get trimmed or are
/// discarded as required.
/// This test received a 12-byte SDU in 3-byte segments, leaving out one of the segments.
/// Then the same SDU is received in 4-byte segments.
/// The content of the final SDU is checked against the original.
/// The test is repeated for all positions of the missing element.
TEST_P(rlc_rx_am_test, rx_partially_overlapping_segments_3_4)
{
  uint32_t sn_state       = 0;
  uint32_t sdu_size       = 12;
  uint32_t segment_size_a = 3;
  uint32_t segment_size_b = 4;

  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 0, 0);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 1, 1);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 2, 2);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 3, 3);
}

/// Verify that reception of partially overlapping segments is properly managed, i.e. segments get trimmed or are
/// discarded as required.
/// This test received a 12-byte SDU in 4-byte segments, leaving out one of the segments.
/// Then the same SDU is received in 3-byte segments.
/// The content of the final SDU is checked against the original.
/// The test is repeated for all positions of the missing element.
TEST_P(rlc_rx_am_test, rx_partially_overlapping_segments_4_3)
{
  uint32_t sn_state       = 0;
  uint32_t sdu_size       = 12;
  uint32_t segment_size_a = 4;
  uint32_t segment_size_b = 3;

  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 0, 0);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 1, 1);
  rx_overlapping_sdu_segments(sn_state, sdu_size, segment_size_a, segment_size_b, 2, 2);
}

/// Verify status prohibit timer:
/// - Read status report which resets status prohibit timer
/// - Rx a PDU with polling bit set
/// - Status required state shall remain false until status prohibit timer expires
TEST_P(rlc_rx_am_test, status_prohibit_timer)
{
  EXPECT_FALSE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 0);

  uint32_t sn_state = 0;
  uint32_t sdu_size = 1;

  // check status report, reset status_prohibit_timer
  rlc_am_status_pdu status_report = rlc->get_status_pdu();
  ue_worker.run_pending_tasks(); // Starting t-StatusProhibit is now defered.
  EXPECT_EQ(status_report.ack_sn, sn_state);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);

  // Create SDU and PDU with full SDU
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, sdu_size, sn_state));
  sn_state++;

  // Change polling bit in first byte of PDU (header)
  *(pdu_list.front().begin()) |= 0b01000000; // set P = 1;

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_list.front())};
  rlc->handle_pdu(std::move(pdu));

  // Status report must not be required as long as status_prohibit_timer is running
  EXPECT_FALSE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 0);

  for (int i = 0; i < config.t_status_prohibit; i++) {
    EXPECT_FALSE(rlc->status_report_required());
    EXPECT_EQ(tester->status_trigger_counter, 0);
    tick();
  }
  EXPECT_TRUE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // reading the size of the status PDU must not change anything on the required status
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);
  EXPECT_TRUE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // check status report, reset status_prohibit_timer
  status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, sn_state);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);

  EXPECT_FALSE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);
}

/// Verify reassembly timer:
/// - Receive SDU in segments, but loose one segment
/// - Let reassembly timer expire
/// - Check that status report is required
/// - Verify the status report NACK's the missing segment
TEST_P(rlc_rx_am_test, reassembly_timer)
{
  uint32_t sn_state     = 0;
  uint32_t sdu_size     = 10;
  uint32_t segment_size = 1;

  // check status report
  rlc_am_status_pdu status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, sn_state);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);

  // Create SDU and PDUs with SDU segments
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
  sn_state++;

  // Push PDUs except for 5th into RLC
  int i = 0;
  for (const byte_buffer& pdu_buf : pdu_list) {
    if (i != 5) {
      byte_buffer_slice pdu = {pdu_buf.deep_copy()};
      rlc->handle_pdu(std::move(pdu));
    }
    i++;
  }

  // Check that nothing was forwarded to upper layer
  EXPECT_EQ(tester->sdu_queue.size(), 0);

  // Let the reassembly timer expire
  for (int j = 0; j < config.t_reassembly; j++) {
    EXPECT_FALSE(rlc->status_report_required());
    EXPECT_EQ(tester->status_trigger_counter, 0);
    tick();
  }

  EXPECT_TRUE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // check status report
  uint32_t nack_size = sn_size == rlc_am_sn_size::size12bits ? rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn
                                                             : rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn;
  status_report      = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, sn_state);
  EXPECT_EQ(status_report.get_packed_size(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + nack_size + rlc_am_nr_status_pdu_sizeof_nack_so);
  EXPECT_EQ(rlc->get_status_pdu_length(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + nack_size + rlc_am_nr_status_pdu_sizeof_nack_so);
  ASSERT_EQ(status_report.get_nacks().size(), 1);
  EXPECT_TRUE(status_report.get_nacks().front().has_so);
  EXPECT_EQ(status_report.get_nacks().front().so_start, 5);
  EXPECT_EQ(status_report.get_nacks().front().so_end, 5);
}

/// Verify reassembly timer is triggered upon reception of PDUs:
///
/// - if t-Reassembly is not running (includes the case t-Reassembly is notify_stop due to actions above):
///   - if RX_Next_Highest> RX_Next +1; or
///   - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
///     with SN = RX_Next before the last byte of all received segments of this SDU:
///
TEST_P(rlc_rx_am_test, when_rx_next_highest_equal_to_rx_next_reassembly_timer_triggered)
{
  uint32_t sn_state     = 0;
  uint32_t sdu_size     = 10;
  uint32_t segment_size = 1;

  // Create SDU and PDUs with SDU segments
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
  sn_state++;

  // Check:
  ///   - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
  ///     with SN = RX_Next before the last byte of all received segments of this SDU:
  // Push PDUs except for 5th into RLC (rx_next=0 rx_next_highest=1)
  int i = 0;
  for (const byte_buffer& pdu_buf : pdu_list) {
    rlc_rx_am_state st = rlc->get_state();
    if (i == 0) {
      ASSERT_EQ(0, st.rx_next_highest);
    } else {
      ASSERT_EQ(1, st.rx_next_highest);
    }
    ASSERT_EQ(0, st.rx_next);
    if (i != 5) {
      byte_buffer_slice pdu = {pdu_buf.deep_copy()};
      rlc->handle_pdu(std::move(pdu));
    }
    i++;
  }

  // Check if t-Reassembly is running
  ASSERT_EQ(true, rlc->is_t_reassembly_running());

  /// Expiration checks are left for their own unit test.
}

/// Verify reassembly timer is triggered upon reception of PDUs:
///
/// - if t-Reassembly is not running (includes the case t-Reassembly is notify_stop due to actions above):
///   - if RX_Next_Highest> RX_Next +1; or
///   - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
///     with SN = RX_Next before the last byte of all received segments of this SDU:
///
TEST_P(rlc_rx_am_test, when_rx_next_highest_larger_then_rx_next_reassembly_timer_triggered)
{
  uint32_t sn_state     = 0;
  uint32_t sdu_size     = 1;
  uint32_t segment_size = 1;
  uint32_t n_sdus       = 10;

  // Create SDU and PDUs with SDU segments
  std::list<std::list<byte_buffer>> pdus = {};
  std::list<byte_buffer>            sdus = {};

  // Create 10 PDUs out of 10 SDUs
  for (uint32_t i = 0; i < n_sdus; i++) {
    std::list<byte_buffer> pdu_list = {};
    byte_buffer            sdu;
    ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
    sn_state++;

    // save original PDUs
    pdus.push_back(std::move(pdu_list));

    // save original SDU
    sdus.push_back(std::move(sdu));
  }

  // Check:
  //   - if RX_Next_Highest> RX_Next +1; or
  // Push PDUs except for 5th into RLC (rx_next=0 rx_next_highest=1)
  int i = 0;
  for (const auto& pdu_it : pdus) {
    if (i != 5) {
      byte_buffer_slice pdu = {pdu_it.front().deep_copy()};
      rlc->handle_pdu(std::move(pdu));
    }
    i++;
  }

  // Check if t-Reassembly is running
  ASSERT_EQ(true, rlc->is_t_reassembly_running());

  /// Expiration checks are left for their own unit test.
}

/// Verify reassembly timer is *not* triggered upon reception of PDUs:
///
/// - if t-Reassembly is not running (includes the case t-Reassembly is notify_stop due to actions above):
///   - if RX_Next_Highest> RX_Next +1; or
///   - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
///     with SN = RX_Next before the last byte of all received segments of this SDU:
///
TEST_P(rlc_rx_am_test, when_rx_next_highest_equal_to_rx_next_but_no_byte_missing_then_reassembly_timer_not_triggered)
{
  uint32_t sn_state     = 0;
  uint32_t sdu_size     = 10;
  uint32_t segment_size = 1;

  // Create SDU and PDUs with SDU segments
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
  sn_state++;

  // Check:
  ///   - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
  ///     with SN = RX_Next before the last byte of all received segments of this SDU:
  // Push PDUs except for 5th into RLC (rx_next=0 rx_next_highest=1)
  int i = 0;
  for (const byte_buffer& pdu_buf : pdu_list) {
    rlc_rx_am_state st = rlc->get_state();
    if (i == 0) {
      ASSERT_EQ(0, st.rx_next_highest);
    } else {
      ASSERT_EQ(1, st.rx_next_highest);
    }
    ASSERT_EQ(0, st.rx_next);
    if (i != 9) {
      byte_buffer_slice pdu = {pdu_buf.deep_copy()};
      rlc->handle_pdu(std::move(pdu));
    }
    i++;
  }

  // Check if t-Reassembly is running
  ASSERT_EQ(false, rlc->is_t_reassembly_running());

  /// Expiration checks are left for their own unit test.
}

/// Verify complex status report (SDU segments (single, sequence, last segment), full SDUs and SDU ranges)
/// - Receive SDU in segments, but loose one segment; two consecutive segments and the final segment
/// - Receive full SDUs, but loose one SDU and a sequence of two consecutive SDUs
/// - Expire reassembly timer
/// - Check status report that for NACK'ed segments (the full SDUs are not yet late)
/// - Receive a new SDU with polling bit set
/// - Check status report has not changed (the full SDUs are still not late)
/// - Expire reassembly timer again - now the full SDUs become late
/// - Check status report for NACK'ed segments, NACK'ed SDU and NACK'ed SDU range
TEST_P(rlc_rx_am_test, status_report)
{
  uint32_t sn_state     = 0;
  uint32_t sdu_size     = 10;
  uint32_t segment_size = 1;

  // check status report
  rlc_am_status_pdu status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, sn_state);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);

  // Create SDU and PDUs with SDU segments
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
  sn_state++;

  // Keep missing PDUs for later
  std::list<byte_buffer> missing_pdus = {};

  // Push PDUs except for 4th, 6th and 7th into RLC
  uint32_t i = 0;
  for (byte_buffer& pdu_buf : pdu_list) {
    if (i != 4 && i != 6 && i != 7 && i != 9) {
      byte_buffer_slice pdu = {std::move(pdu_buf)};
      rlc->handle_pdu(std::move(pdu));
    } else {
      missing_pdus.push_back(std::move(pdu_buf));
    }
    i++;
  }

  // Check that nothing was forwarded to upper layer
  EXPECT_EQ(tester->sdu_queue.size(), 0);

  // Create further SDUs and PDUs with full SDUs
  uint32_t n_full_sdus = 10;
  for (i = 0; i < n_full_sdus; i++) {
    pdu_list.clear();
    ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, sdu_size, sn_state));

    // Push PDUs except for 4th, 6th and 7th into RLC
    if (sn_state != 4 && sn_state != 6 && sn_state != 7) {
      byte_buffer_slice pdu = {std::move(pdu_list.front())};
      rlc->handle_pdu(std::move(pdu));
    } else {
      missing_pdus.push_back(std::move(pdu_list.front()));
    }
    sn_state++;
  }

  // Check complete SDUs were forwarded to upper layer
  EXPECT_EQ(tester->sdu_queue.size(), 7);

  // Let the reassembly timer expire (advance rx_highest_status to 4 and rx_next_status_trigger to 11)
  for (int t = 0; t < config.t_reassembly; t++) {
    EXPECT_FALSE(rlc->status_report_required());
    EXPECT_EQ(tester->status_trigger_counter, 0);
    tick();
  }

  EXPECT_TRUE(rlc->status_report_required());
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // Check status report
  uint32_t nack_size = sn_size == rlc_am_sn_size::size12bits ? rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn
                                                             : rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn;
  status_report      = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, 4);
  EXPECT_EQ(status_report.get_packed_size(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + 3 * (nack_size + rlc_am_nr_status_pdu_sizeof_nack_so));
  EXPECT_EQ(rlc->get_status_pdu_length(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + 3 * (nack_size + rlc_am_nr_status_pdu_sizeof_nack_so));
  ASSERT_EQ(status_report.get_nacks().size(), 3);

  EXPECT_TRUE(status_report.get_nacks().at(0).has_so);
  EXPECT_EQ(status_report.get_nacks().at(0).so_start, 4);
  EXPECT_EQ(status_report.get_nacks().at(0).so_end, 4);

  EXPECT_TRUE(status_report.get_nacks().at(1).has_so);
  EXPECT_EQ(status_report.get_nacks().at(1).so_start, 6);
  EXPECT_EQ(status_report.get_nacks().at(1).so_end, 7);

  EXPECT_TRUE(status_report.get_nacks().at(2).has_so);
  EXPECT_EQ(status_report.get_nacks().at(2).so_start, 9);
  EXPECT_EQ(status_report.get_nacks().at(2).so_end, rlc_am_status_nack::so_end_of_sdu);

  // Create SDU and PDU with full SDU (set poll bit)
  pdu_list.clear();
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, sdu_size, sn_state));
  sn_state++;

  // Change polling bit in first byte of PDU (header)
  *(pdu_list.front().begin()) |= 0b01000000; // set P = 1;

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_list.front())};
  rlc->handle_pdu(std::move(pdu));

  EXPECT_FALSE(rlc->status_report_required()); // status prohibit timer is not yet expired, regardless we read status
  EXPECT_EQ(tester->status_trigger_counter, 1);

  // Check status report
  status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, 4);
  EXPECT_EQ(status_report.get_packed_size(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + 3 * (nack_size + rlc_am_nr_status_pdu_sizeof_nack_so));
  EXPECT_EQ(rlc->get_status_pdu_length(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + 3 * (nack_size + rlc_am_nr_status_pdu_sizeof_nack_so));
  ASSERT_EQ(status_report.get_nacks().size(), 3);

  EXPECT_TRUE(status_report.get_nacks().at(0).has_so);
  EXPECT_EQ(status_report.get_nacks().at(0).so_start, 4);
  EXPECT_EQ(status_report.get_nacks().at(0).so_end, 4);

  EXPECT_TRUE(status_report.get_nacks().at(1).has_so);
  EXPECT_EQ(status_report.get_nacks().at(1).so_start, 6);
  EXPECT_EQ(status_report.get_nacks().at(1).so_end, 7);

  EXPECT_TRUE(status_report.get_nacks().at(2).has_so);
  EXPECT_EQ(status_report.get_nacks().at(2).so_start, 9);
  EXPECT_EQ(status_report.get_nacks().at(2).so_end, rlc_am_status_nack::so_end_of_sdu);

  // Let the reassembly timer expire (advance rx_highest_status to 12)
  for (int t = 0; t < config.t_reassembly; t++) {
    tick();
  }

  EXPECT_EQ(tester->status_trigger_counter, 2);

  // Check status report
  status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, 12);
  EXPECT_EQ(status_report.get_packed_size(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + 3 * (nack_size + rlc_am_nr_status_pdu_sizeof_nack_so) +
                2 * nack_size + rlc_am_nr_status_pdu_sizeof_nack_range);
  EXPECT_EQ(rlc->get_status_pdu_length(),
            rlc_am_nr_status_pdu_sizeof_header_ack_sn + 3 * (nack_size + rlc_am_nr_status_pdu_sizeof_nack_so) +
                2 * nack_size + rlc_am_nr_status_pdu_sizeof_nack_range);
  ASSERT_EQ(status_report.get_nacks().size(), 5);
  EXPECT_EQ(status_report.get_nacks().at(0).nack_sn, 0);
  EXPECT_TRUE(status_report.get_nacks().at(0).has_so);
  EXPECT_EQ(status_report.get_nacks().at(0).so_start, 4);
  EXPECT_EQ(status_report.get_nacks().at(0).so_end, 4);

  EXPECT_EQ(status_report.get_nacks().at(1).nack_sn, 0);
  EXPECT_TRUE(status_report.get_nacks().at(0).has_so);
  EXPECT_EQ(status_report.get_nacks().at(1).so_start, 6);
  EXPECT_EQ(status_report.get_nacks().at(1).so_end, 7);

  EXPECT_EQ(status_report.get_nacks().at(2).nack_sn, 0);
  EXPECT_TRUE(status_report.get_nacks().at(2).has_so);
  EXPECT_EQ(status_report.get_nacks().at(2).so_start, 9);
  EXPECT_EQ(status_report.get_nacks().at(2).so_end, rlc_am_status_nack::so_end_of_sdu);

  EXPECT_EQ(status_report.get_nacks().at(3).nack_sn, 4);
  EXPECT_FALSE(status_report.get_nacks().at(3).has_so);

  EXPECT_EQ(status_report.get_nacks().at(4).nack_sn, 6);
  EXPECT_FALSE(status_report.get_nacks().at(4).has_so);
  EXPECT_TRUE(status_report.get_nacks().at(4).has_nack_range);
  EXPECT_EQ(status_report.get_nacks().at(4).nack_range, 2);
}

/// Verify in-order Rx of full SDUs
/// Example: [0][1][2][3][4]
TEST_P(rlc_rx_am_test, rx_without_segmentation)
{
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_full_sdus(sn, n_sdus, 1, /* reverse_sdus = */ false);
  rx_full_sdus(sn, n_sdus, 5, /* reverse_sdus = */ false);
}

/// Verify reverse-order Rx of full SDUs
/// Example: [4][3][2][1][0]
TEST_P(rlc_rx_am_test, rx_reverse_without_segmentation)
{
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_full_sdus(sn, n_sdus, 1, /* reverse_sdus = */ true);
  rx_full_sdus(sn, n_sdus, 5, /* reverse_sdus = */ true);
}

/// Verify in-order Rx of SDU segments
/// Example: [0 0:0][0 1:1][1 0:0][1 1:1][2 0:0][2 1:1][3 0:0][3 1:1][4 0:0][4 1:1]
TEST_P(rlc_rx_am_test, rx_with_segmentation)
{
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_sdu_segments(sn, n_sdus, 2, 1, /* reverse_sdus = */ false, /* reverse_segments = */ false);
  rx_sdu_segments(sn, n_sdus, 8, 3, /* reverse_sdus = */ false, /* reverse_segments = */ false);
}

/// Verify reverse-order Rx of SDU segments but the segments of each SDU are transmitted in order
/// Example: [4 0:0][4 1:1][3 0:0][3 1:1][2 0:0][2 1:1][1 0:0][1 1:1][0 0:0][0 1:1]
TEST_P(rlc_rx_am_test, rx_reverse_with_segmentation)
{
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_sdu_segments(sn, n_sdus, 2, 1, /* reverse_sdus = */ true, /* reverse_segments = */ false);
  rx_sdu_segments(sn, n_sdus, 8, 3, /* reverse_sdus = */ true, /* reverse_segments = */ false);
}

/// Verify in-order Rx of SDU segments but the segments of each SDU are transmitted in reverse order
/// Example: [0 1:1][0 0:0][1 1:1][1 0:0][2 1:1][2 0:0][3 0:0][3 1:1][3 0:0][4 1:1][4 0:0]
TEST_P(rlc_rx_am_test, rx_with_reversed_segmentation)
{
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_sdu_segments(sn, n_sdus, 2, 1, /* reverse_sdus = */ false, /* reverse_segments = */ true);
  rx_sdu_segments(sn, n_sdus, 8, 3, /* reverse_sdus = */ false, /* reverse_segments = */ true);
}

/// Verify reverse-order Rx of SDU segments and the segments of each SDU are transmitted in reverse order
/// Example: [4 1:1][4 0:0][3 0:0][3 1:1][3 0:0][2 1:1][2 0:0][1 1:1][1 0:0][0 1:1][0 0:0]
TEST_P(rlc_rx_am_test, rx_reverse_with_reversed_segmentation)
{
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_sdu_segments(sn, n_sdus, 2, 1, /* reverse_sdus = */ true, /* reverse_segments = */ true);
  rx_sdu_segments(sn, n_sdus, 8, 3, /* reverse_sdus = */ true, /* reverse_segments = */ true);
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

INSTANTIATE_TEST_SUITE_P(rlc_rx_am_test_each_sn_size,
                         rlc_rx_am_test,
                         ::testing::Values(rlc_am_sn_size::size12bits, rlc_am_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
