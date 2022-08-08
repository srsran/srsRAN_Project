/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/rlc/rlc_am_entity.h"
#include <gtest/gtest.h>

using namespace srsgnb;

template <std::size_t N>
byte_buffer make_byte_buffer_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer sdu = {tv};
  return sdu;
}

template <std::size_t N>
byte_buffer_slice_chain make_rlc_byte_buffer_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer             buf = {tv};
  byte_buffer_slice_chain pdu;
  pdu.push_back(std::move(buf));
  return pdu;
}

/// Mocking class of the surrounding layers invoked by the RLC.
class rlc_test_frame : public rlc_rx_upper_layer_data_notifier,
                       public rlc_tx_upper_layer_control_notifier,
                       public rlc_tx_buffer_state_update_notifier
{
public:
  rlc_sdu_queue sdu_queue;
  uint32_t      sdu_counter = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_slice sdu) override
  {
    rlc_sdu boxed_sdu(sdu_counter++, std::move(sdu));
    sdu_queue.write(boxed_sdu);
  }
  void on_ack_received() override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override{};

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr) override {}
};

/// Fixture class for RLC AM Tx tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class rlc_am_tx_test : public ::testing::Test, public ::testing::WithParamInterface<rlc_am_sn_size>
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
    logger.info("Creating RLC AM ({} bit)", to_number(sn_size));

    sn_size = sn_size_;

    // Set Rx config
    config.rx                    = std::make_unique<rlc_rx_am_config>(rlc_rx_am_config{});
    config.rx->sn_field_length   = sn_size;
    config.rx->t_reassembly      = 35;
    config.rx->t_status_prohibit = 8;

    // Set Tx config
    config.tx                  = std::make_unique<rlc_tx_am_config>(rlc_tx_am_config{});
    config.tx->sn_field_length = sn_size;
    config.tx->t_poll_retx     = 45;
    config.tx->max_retx_thresh = 4;
    config.tx->poll_pdu        = 4;
    config.tx->poll_byte       = 25;

    // Create RLC entities
    rlc1 = std::make_unique<rlc_am_entity>(
        du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester1, tester1, tester1, timers);
    rlc2 = std::make_unique<rlc_am_entity>(
        du_ue_index_t::MIN_DU_UE_INDEX, lcid_t::LCID_SRB0, config, tester2, tester2, tester2, timers);

    // Bind interfaces
    rlc1_rx_lower = rlc2->get_rx_pdu_handler();
    rlc1_tx_upper = rlc1->get_tx_sdu_handler();
    rlc1_tx_lower = rlc1->get_tx_pdu_transmitter();
    rlc2_rx_lower = rlc2->get_rx_pdu_handler();
    rlc2_tx_upper = rlc1->get_tx_sdu_handler();
    rlc2_tx_lower = rlc2->get_tx_pdu_transmitter();
  }

  /// \brief Obtains full RLC AMD PDUs from generated SDUs that are passed through an RLC AM entity
  /// \param[out] out_pdus Pre-allocated array of size n_pdus for the resulting RLC AMD PDUs
  /// \param[in] n_pdus Length of the out_pdus array
  /// \param[in] sdu_size Size of SDU that is passed through RLC AM entity
  void tx_full_pdus(byte_buffer_slice_chain* out_pdus, uint32_t n_pdus, uint32_t sdu_size = 1)
  {
    // Push "n_pdus" SDUs into RLC1
    byte_buffer sdu_bufs[n_pdus];
    for (uint32_t i = 0; i < n_pdus; i++) {
      sdu_bufs[i] = byte_buffer();
      for (uint32_t k = 0; k < sdu_size; ++k) {
        sdu_bufs[i].append(i + k);
      }

      // write SDU into upper end
      rlc_sdu sdu = {i, sdu_bufs[i].deep_copy()}; // no std::move - keep local copy for later comparison
      rlc1_tx_upper->handle_sdu(std::move(sdu));
    }

    uint32_t header_size         = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    uint32_t data_pdu_size       = header_size + sdu_size;
    uint32_t expect_buffer_state = n_pdus * data_pdu_size;

    // Read "n_pdus" PDUs from RLC1
    for (uint32_t i = 0; i < n_pdus; i++) {
      EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), expect_buffer_state - i * data_pdu_size);
      out_pdus[i] = rlc1_tx_lower->pull_pdu(data_pdu_size);
      EXPECT_EQ(out_pdus[i].length(), data_pdu_size);
      EXPECT_TRUE(
          std::equal(out_pdus[i].begin() + header_size, out_pdus[i].end(), sdu_bufs[i].begin(), sdu_bufs[i].end()));
    }
    EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  }

  srslog::basic_logger&          logger  = srslog::fetch_basic_logger("TEST", false);
  rlc_am_sn_size                 sn_size = GetParam();
  rlc_am_config                  config;
  timer_manager                  timers;
  rlc_test_frame                 tester1, tester2;
  std::unique_ptr<rlc_am_entity> rlc1, rlc2;
  rlc_rx_pdu_handler*            rlc1_rx_lower = nullptr;
  rlc_tx_sdu_handler*            rlc1_tx_upper = nullptr;
  rlc_tx_pdu_transmitter*        rlc1_tx_lower = nullptr;
  rlc_rx_pdu_handler*            rlc2_rx_lower = nullptr;
  rlc_tx_sdu_handler*            rlc2_tx_upper = nullptr;
  rlc_tx_pdu_transmitter*        rlc2_tx_lower = nullptr;
};

TEST_P(rlc_am_tx_test, create_new_entity)
{
  init(GetParam());
  EXPECT_NE(rlc1_rx_lower, nullptr);
  EXPECT_NE(rlc1_tx_upper, nullptr);
  ASSERT_NE(rlc1_tx_lower, nullptr);
  EXPECT_NE(rlc2_rx_lower, nullptr);
  EXPECT_NE(rlc2_tx_upper, nullptr);
  ASSERT_NE(rlc2_tx_lower, nullptr);

  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
  EXPECT_EQ(rlc2_tx_lower->get_buffer_state(), 0);
}

TEST_P(rlc_am_tx_test, tx_without_segmentation)
{
  init(GetParam());
  const uint32_t          n_pdus = 5;
  byte_buffer_slice_chain pdus[n_pdus];

  tx_full_pdus(pdus, n_pdus, 1);
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);

  tx_full_pdus(pdus, n_pdus, 5);
  EXPECT_EQ(rlc1_tx_lower->get_buffer_state(), 0);
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

INSTANTIATE_TEST_SUITE_P(rlc_am_tx_test_all_sn_sizes,
                         rlc_am_tx_test,
                         ::testing::Values(rlc_am_sn_size::size12bits, rlc_am_sn_size::size18bits),
                         test_param_info_to_string);
