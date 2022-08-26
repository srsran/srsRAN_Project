/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/rlc/rlc_rx_am_entity.h"
#include <gtest/gtest.h>
#include <list>
#include <queue>
#include <utility>

using namespace srsgnb;

/// Mocking class of the surrounding layers invoked by the RLC AM Rx entity.
class rlc_rx_am_test_frame : public rlc_rx_upper_layer_data_notifier, public rlc_tx_am_status_handler
{
public:
  std::queue<byte_buffer_slice> sdu_queue;
  uint32_t                      sdu_counter = 0;
  rlc_am_sn_size                sn_size;
  rlc_am_status_pdu             status;

  rlc_rx_am_test_frame(rlc_am_sn_size sn_size) : sn_size(sn_size), status(sn_size) {}

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_slice sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }

  // rlc_tx_am_status_handler interface
  virtual void handle_status_pdu(rlc_am_status_pdu status) override { this->status = status; }
};

/// Fixture class for RLC AM Rx tests.
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class rlc_rx_am_test : public ::testing::Test, public ::testing::WithParamInterface<rlc_am_sn_size>
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
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Initializes fixture according to size sequence number size
  /// \param sn_size_ size of the sequence number
  void init(rlc_am_sn_size sn_size_)
  {
    logger.info("Creating RLC Rx AM entity ({} bit)", to_number(sn_size));

    sn_size = sn_size_;

    // Set Rx config
    config.sn_field_length   = sn_size;
    config.t_reassembly      = 35;
    config.t_status_prohibit = 8;

    // Create test frame
    tester = std::make_unique<rlc_rx_am_test_frame>(config.sn_field_length);

    // Create RLC AM RX entity
    rlc =
        std::make_unique<rlc_rx_am_entity>(du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, *tester, timers);

    // Bind AM Tx/Rx interconnect
    rlc->set_status_handler(tester.get());
  }

  /// \brief Creates a byte_buffer serving as SDU for RLC
  ///
  /// The produced SDU contains an incremental sequence of bytes starting with the value given by first_byte,
  /// i.e. if first_byte = 0xfc, the SDU will be 0xfc 0xfe 0xff 0x00 0x01 ...
  /// \param sdu_size Size of the SDU
  /// \param first_byte Value of the first byte
  /// \return the produced SDU as a byte_buffer
  byte_buffer create_sdu(uint32_t sdu_size, uint8_t first_byte = 0) const
  {
    byte_buffer sdu_buf;
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_buf.append(first_byte + k);
    }
    return sdu_buf;
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
      rlc_am_write_data_pdu_header(hdr, pdu_buf);
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
      EXPECT_TRUE(std::equal(tester->sdu_queue.front().begin(),
                             tester->sdu_queue.front().end(),
                             sdu_originals.front().begin(),
                             sdu_originals.front().end()));
      tester->sdu_queue.pop();
      sdu_originals.pop_front();
    }
    EXPECT_EQ(tester->sdu_queue.size(), 0);
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
      EXPECT_TRUE(std::equal(tester->sdu_queue.front().begin(),
                             tester->sdu_queue.front().end(),
                             sdu_originals.front().begin(),
                             sdu_originals.front().end()));
      tester->sdu_queue.pop();
      sdu_originals.pop_front();
    }
    EXPECT_EQ(tester->sdu_queue.size(), 0);
  }

  srslog::basic_logger&                 logger  = srslog::fetch_basic_logger("TEST", false);
  rlc_am_sn_size                        sn_size = GetParam();
  rlc_rx_am_config                      config;
  timer_manager                         timers;
  std::unique_ptr<rlc_rx_am_test_frame> tester;
  std::unique_ptr<rlc_rx_am_entity>     rlc;
};

TEST_P(rlc_rx_am_test, create_new_entity)
{
  init(GetParam());
  EXPECT_NE(rlc, nullptr);
}

TEST_P(rlc_rx_am_test, read_initial_status)
{
  init(GetParam());
  EXPECT_FALSE(rlc->status_report_required());
  rlc_am_status_pdu status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, 0);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);
}

TEST_P(rlc_rx_am_test, window_checker_lower_edge)
{
  init(GetParam());
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

TEST_P(rlc_rx_am_test, window_checker_upper_edge)
{
  init(GetParam());

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

TEST_P(rlc_rx_am_test, rx_valid_control_pdu)
{
  init(GetParam());

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
  rlc->handle_pdu(pdu);

  // Pick and verify the received status PDU on the other end
  EXPECT_EQ(tester->status.ack_sn, status.ack_sn);
  ASSERT_EQ(tester->status.get_nacks().size(), status.get_nacks().size());
  EXPECT_EQ(tester->status.get_nacks().front().nack_sn, nack.nack_sn);
}

TEST_P(rlc_rx_am_test, rx_invalid_control_pdu)
{
  init(GetParam());

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
  rlc->handle_pdu(pdu);

  // Pick and verify the received status PDU on the other end
  EXPECT_EQ(tester->status.ack_sn, INVALID_RLC_SN);
  EXPECT_EQ(tester->status.get_nacks().size(), 0);
}

TEST_P(rlc_rx_am_test, rx_short_data_pdu)
{
  init(GetParam());

  EXPECT_FALSE(rlc->status_report_required());

  // Create too short data PDU with polling bit set
  byte_buffer pdu_buf = {};
  pdu_buf.append(0b11000000); // D/C = 1; P = 1

  // Push into RLC
  byte_buffer_slice pdu = {std::move(pdu_buf)};
  rlc->handle_pdu(pdu);

  // Check if polling bit of malformed PDU was properly ignored
  EXPECT_FALSE(rlc->status_report_required());
}

TEST_P(rlc_rx_am_test, rx_polling_bit_sn_inside_rx_window)
{
  init(GetParam());

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
  rlc->handle_pdu(pdu);

  // Check if polling bit of malformed PDU was properly ignored
  EXPECT_TRUE(rlc->status_report_required());

  // Check if SDU was properly unpacked and forwarded
  ASSERT_EQ(tester->sdu_queue.size(), 1);
  EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
  EXPECT_TRUE(std::equal(tester->sdu_queue.front().begin(), tester->sdu_queue.front().end(), sdu.begin(), sdu.end()));
  tester->sdu_queue.pop();
}

TEST_P(rlc_rx_am_test, rx_polling_bit_sn_outside_rx_window)
{
  init(GetParam());

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
  rlc->handle_pdu(pdu);

  // Check if polling bit was considered, despite out-of-window SN
  EXPECT_TRUE(rlc->status_report_required());

  // Check if SDU was properly ignored
  ASSERT_EQ(tester->sdu_queue.size(), 0);
}

TEST_P(rlc_rx_am_test, rx_polling_bit_SDU_duplicate)
{
  init(GetParam());

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
  rlc->handle_pdu(pdu);

  // Check if polling bit has not changed
  EXPECT_FALSE(rlc->status_report_required());

  // Check if SDU was properly unpacked and forwarded
  ASSERT_EQ(tester->sdu_queue.size(), 1);
  EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
  EXPECT_TRUE(std::equal(tester->sdu_queue.front().begin(), tester->sdu_queue.front().end(), sdu.begin(), sdu.end()));
  tester->sdu_queue.pop();

  // Change polling bit in first byte of PDU (header)
  *(pdu_list.front().begin()) |= 0b01000000; // set P = 1;

  // Push into RLC
  pdu = {pdu_list.front().deep_copy()};
  rlc->handle_pdu(pdu);

  // Check if polling bit was considered, despite duplicate SN
  EXPECT_TRUE(rlc->status_report_required());

  // Check if duplicate SDU was properly ignored
  ASSERT_EQ(tester->sdu_queue.size(), 0);
}

TEST_P(rlc_rx_am_test, rx_duplicate_segments)
{
  init(GetParam());

  uint32_t sn_state     = 0;
  uint32_t sdu_size     = 10;
  uint32_t segment_size = 1;

  // check status report
  rlc_am_status_pdu status_report = rlc->get_status_pdu();
  EXPECT_EQ(status_report.ack_sn, sn_state);
  EXPECT_EQ(status_report.get_nacks().size(), 0);
  EXPECT_EQ(status_report.get_packed_size(), 3);
  EXPECT_EQ(rlc->get_status_pdu_length(), 3);

  // Create SDU and PDUs with full SDU segments
  std::list<byte_buffer> pdu_list = {};
  byte_buffer            sdu;
  ASSERT_NO_FATAL_FAILURE(create_pdus(pdu_list, sdu, sn_state, sdu_size, segment_size, sn_state));
  sn_state++;

  // Push PDUs except for 5th into RLC
  int i = 0;
  for (const byte_buffer& pdu_buf : pdu_list) {
    if (i != 5) {
      byte_buffer_slice pdu = {pdu_buf.deep_copy()};
      rlc->handle_pdu(pdu);
    }
    i++;
  }

  // Check that nothing was forwarded to upper layer
  EXPECT_EQ(tester->sdu_queue.size(), 0);

  // Push all PDUs again; check that nothing is forwarded to upper layer before except after Rx of 5th segment
  i = 0;
  for (const byte_buffer& pdu_buf : pdu_list) {
    byte_buffer_slice pdu = {pdu_buf.deep_copy()};
    rlc->handle_pdu(pdu);
    if (i == 5) {
      // check if SDU has been assembled correctly
      ASSERT_EQ(tester->sdu_queue.size(), 1);
      EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
      EXPECT_TRUE(
          std::equal(tester->sdu_queue.front().begin(), tester->sdu_queue.front().end(), sdu.begin(), sdu.end()));
      tester->sdu_queue.pop();
    } else {
      EXPECT_EQ(tester->sdu_queue.size(), 0);
    }
    i++;
  }
}

TEST_P(rlc_rx_am_test, rx_without_segmentation)
{
  init(GetParam());
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_full_sdus(sn, n_sdus, 1, /* reverse_sdus = */ false);
  rx_full_sdus(sn, n_sdus, 5, /* reverse_sdus = */ false);
}

TEST_P(rlc_rx_am_test, rx_reverse_without_segmentation)
{
  init(GetParam());
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_full_sdus(sn, n_sdus, 1, /* reverse_sdus = */ true);
  rx_full_sdus(sn, n_sdus, 5, /* reverse_sdus = */ true);
}

TEST_P(rlc_rx_am_test, rx_with_segmentation)
{
  init(GetParam());
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_sdu_segments(sn, n_sdus, 2, 1, /* reverse_sdus = */ false, /* reverse_segments = */ false);
  rx_sdu_segments(sn, n_sdus, 8, 3, /* reverse_sdus = */ false, /* reverse_segments = */ false);
}

TEST_P(rlc_rx_am_test, rx_reverse_with_segmentation)
{
  init(GetParam());
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_sdu_segments(sn, n_sdus, 2, 1, /* reverse_sdus = */ true, /* reverse_segments = */ false);
  rx_sdu_segments(sn, n_sdus, 8, 3, /* reverse_sdus = */ true, /* reverse_segments = */ false);
}

TEST_P(rlc_rx_am_test, rx_with_reversed_segmentation)
{
  init(GetParam());
  const uint32_t n_sdus = 5;
  uint32_t       sn     = 0;

  rx_sdu_segments(sn, n_sdus, 2, 1, /* reverse_sdus = */ false, /* reverse_segments = */ true);
  rx_sdu_segments(sn, n_sdus, 8, 3, /* reverse_sdus = */ false, /* reverse_segments = */ true);
}

TEST_P(rlc_rx_am_test, rx_reverse_with_reversed_segmentation)
{
  init(GetParam());
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
