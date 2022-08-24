/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../lib/pdcp/pdcp_entity_rx.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsgnb {

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_rx_test_frame : public pdcp_rx_upper_data_notifier, public pdcp_rx_upper_control_notifier
{
public:
  std::queue<byte_buffer> sdu_queue   = {};
  uint32_t                sdu_counter = 0;

  /// PDCP RX upper layer data notifier
  void on_new_sdu(byte_buffer sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }

  /// PDCP RX upper layer control notifier
  void on_integrity_failure() override {}

  /// PDCP TX/RX upper layer control notifier
  void on_protocol_failure() override {}
};

/// Fixture class for PDCP tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_rx_test : public ::testing::Test, public ::testing::WithParamInterface<pdcp_sn_size>
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("PDCP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  /// \brief Initializes fixture according to size sequence number size
  /// \param sn_size_ size of the sequence number
  void init(pdcp_sn_size sn_size_)
  {
    logger.info("Creating PDCP RX ({} bit)", to_number(sn_size_));

    sn_size = sn_size_;

    // Set Rx config
    config.rb_type               = pdcp_rb_type::drb;
    config.rlc_mode              = pdcp_rlc_mode::am;
    config.sn_size               = sn_size;
    config.out_of_order_delivery = false;
    config.t_reordering          = pdcp_t_reordering::ms10;

    // Create PDCP RX entity
    pdcp_rx = std::make_unique<pdcp_entity_rx>(0, LCID_SRB1, config, test_frame);
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  pdcp_sn_size                sn_size = GetParam();
  pdcp_config::pdcp_rx_config config  = {};
  timer_manager               timers;
  pdcp_rx_test_frame          test_frame = {};

  std::unique_ptr<pdcp_entity_rx> pdcp_rx;
  pdcp_rx_lower_interface*        pdcp_rx_lower = nullptr;
};
} // namespace srsgnb
