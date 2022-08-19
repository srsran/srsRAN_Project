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

  /// \brief Creates a byte_buffer containing a RLC AMD PDU with a full RLC SDU
  ///
  /// The produced SDU contains an incremental sequence of bytes starting with the value given by first_byte,
  /// i.e. if first_byte = 0xfc, the PDU will be <header> 0xfc 0xfe 0xff 0x00 0x01 ...
  /// \param sn The sequence number to be put in the header
  /// \param sdu_size Size of the SDU
  /// \param first_byte Value of the first byte
  /// \return the produced PDU as a byte_buffer
  byte_buffer create_pdu_with_full_sdu(uint32_t sn, uint32_t sdu_size, uint8_t first_byte = 0) const
  {
    byte_buffer       pdu_buf;
    rlc_am_pdu_header hdr = {};
    hdr.dc                = rlc_dc_field::data;
    hdr.p                 = 0;
    hdr.si                = rlc_si_field::full_sdu;
    hdr.sn_size           = config.sn_field_length;
    hdr.sn                = sn;
    hdr.so                = 0;
    logger.debug("AMD PDU header: {}", hdr);
    rlc_am_write_data_pdu_header(hdr, pdu_buf);
    pdu_buf.append(create_sdu(sdu_size, first_byte));
    return pdu_buf;
  }

  /// \brief Creates a list of RLC AMD PDU(s) containing either one RLC SDU or multiple RLC SDU segments
  /// \param sn The sequence number to be put in the header
  /// \param sdu_size Size of the SDU
  /// \param segment_size Maximum payload size of each SDU or SDU segment
  /// \param first_byte Value of the first SDU payload byte
  /// \return A pair consisting of a RLC AMD PDU list and the associated RLC SDU
  std::pair<std::list<byte_buffer>, byte_buffer>
  create_pdus_with_sdu_segments(uint32_t sn, uint32_t sdu_size, uint32_t segment_size, uint8_t first_byte = 0) const
  {
    if (sdu_size == 0) {
      sdu_size = 1;
    }
    if (segment_size == 0) {
      segment_size = sdu_size;
    }

    byte_buffer            sdu_buf  = create_sdu(sdu_size, first_byte);
    std::list<byte_buffer> pdu_list = {};
    byte_buffer_view       rest     = {sdu_buf};

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

    return {std::move(pdu_list), std::move(sdu_buf)};
  }

  /// \brief Obtains RLC AMD PDUs from generated SDUs that are passed through an RLC AM entity
  /// \param[out] out_pdus Pre-allocated array of size n_pdus for the resulting RLC AMD PDUs
  /// \param[in] n_pdus Length of the out_pdus array
  /// \param[in] sdu_size Size of SDU that is passed through RLC AM entity

  /// \brief Injects RLC AMD PDUs into the RLC AM entity starting from Sequence number sn_start
  /// \param[out] injected_pdus Pre-allocated array of size n_pdus for the resulting RLC AMD PDUs
  /// \param[in] n_pdus Length of the injected_pdus array
  /// \param[in] sn_state Reference to the sequence number for the first PDU. Will be incremented for each PDU
  /// \param[in] sdu_size Size of the SDU payload in each PDU that is pushed into the RLC AM entity
  void rx_full_pdus(byte_buffer* injected_pdus, uint32_t n_pdus, uint32_t& sn_state, uint32_t sdu_size = 1)
  {
    // check status report
    rlc_am_status_pdu status_report = rlc->get_status_pdu();
    EXPECT_EQ(status_report.ack_sn, sn_state);
    EXPECT_EQ(status_report.get_nacks().size(), 0);
    EXPECT_EQ(status_report.get_packed_size(), 3);
    EXPECT_EQ(rlc->get_status_pdu_length(), 3);

    // Create and push "n_pdus" PDUs into RLC
    for (uint32_t i = 0; i < n_pdus; i++) {
      std::pair<std::list<byte_buffer>, byte_buffer> pair =
          create_pdus_with_sdu_segments(sn_state, sdu_size, sdu_size, sn_state);
      injected_pdus[i] = std::move(pair.first.front());
      sn_state++;

      // write PDU into lower end
      byte_buffer_slice pdu = {injected_pdus[i].deep_copy()}; // no std::move - we need to return the injected PDUs
      rlc->handle_pdu(std::move(pdu));

      // check status report
      status_report = rlc->get_status_pdu();
      EXPECT_EQ(status_report.ack_sn, sn_state);
      EXPECT_EQ(status_report.get_nacks().size(), 0);
      EXPECT_EQ(status_report.get_packed_size(), 3);
      EXPECT_EQ(rlc->get_status_pdu_length(), 3);
    }

    uint32_t header_size = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;

    // Read "n_pdus" SDUs from upper layer
    ASSERT_EQ(tester->sdu_queue.size(), n_pdus);
    for (uint32_t i = 0; i < n_pdus; i++) {
      EXPECT_EQ(tester->sdu_queue.front().length(), sdu_size);
      EXPECT_TRUE(std::equal(tester->sdu_queue.front().begin(),
                             tester->sdu_queue.front().end(),
                             injected_pdus[i].begin() + header_size,
                             injected_pdus[i].end()));
      tester->sdu_queue.pop();
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

TEST_P(rlc_rx_am_test, rx_without_segmentation)
{
  init(GetParam());
  const uint32_t n_pdus = 5;
  uint32_t       sn     = 0;
  byte_buffer    pdus[n_pdus];

  rx_full_pdus(pdus, n_pdus, sn, 1);
  rx_full_pdus(pdus, n_pdus, sn, 5);
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
