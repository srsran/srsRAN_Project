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

#include "../../lib/pdcp/pdcp_entity_tx.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsgnb {

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_tx_test_frame : public pdcp_tx_lower_notifier, public pdcp_tx_upper_control_notifier
{
public:
  std::queue<byte_buffer> pdu_queue   = {};
  uint32_t                pdu_counter = 0;

  /// PDCP TX upper layer control notifier
  void on_max_hfn_reached() final {}
  void on_protocol_failure() final {}

  /// PDCP TX lower layer data notifier
  void on_new_pdu(byte_buffer pdu) final {}
  void on_discard_pdu(uint32_t count) final {}
};

/// Fixture class for PDCP tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_tx_test : public ::testing::Test, public ::testing::WithParamInterface<pdcp_sn_size>
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
    logger.info("Creating PDCP TX ({} bit)", to_number(sn_size_));

    sn_size = sn_size_;

    // Set TX config
    config.rb_type       = pdcp_rb_type::drb;
    config.rlc_mode      = pdcp_rlc_mode::am;
    config.sn_size       = sn_size;
    config.discard_timer = pdcp_discard_timer::ms10;

    // Create RLC entities
    pdcp_tx = std::make_unique<pdcp_entity_tx>(0, LCID_SRB1, config, test_frame, test_frame);
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  pdcp_sn_size                sn_size = GetParam();
  pdcp_config::pdcp_tx_config config  = {};
  timer_manager               timers;
  pdcp_tx_test_frame          test_frame = {};

  std::unique_ptr<pdcp_entity_tx> pdcp_tx;
};
} // namespace srsgnb
