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
#include "pdcp_test_vectors.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsgnb {

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_rx_test_frame : public pdcp_rx_upper_data_notifier, public pdcp_rx_upper_control_notifier
{
public:
  std::queue<byte_buffer> sdu_queue              = {};
  uint32_t                sdu_counter            = 0;
  uint32_t                integrity_fail_counter = 0;
  uint32_t                nof_max_count_reached  = 0;
  uint32_t                nof_protocol_failure   = 0;

  /// PDCP RX upper layer data notifier
  void on_new_sdu(byte_buffer sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }

  /// PDCP RX upper layer control notifier
  void on_integrity_failure() override { integrity_fail_counter++; }
  void on_protocol_failure() override { nof_protocol_failure++; }
  void on_max_count_reached() override { nof_max_count_reached++; }
};

/// Fixture class for PDCP tests
class pdcp_rx_test_helper
{
protected:
  /// \brief Initializes fixture according to size sequence number size
  /// \param sn_size_ size of the sequence number
  void init(pdcp_sn_size      sn_size_,
            pdcp_t_reordering t_reordering = pdcp_t_reordering::ms10,
            pdcp_max_count    max_count    = {pdcp_rx_default_max_count_notify, pdcp_rx_default_max_count_hard})
  {
    logger.info("Creating PDCP RX ({} bit)", to_number(sn_size_));

    sn_size = sn_size_;

    // Set Rx config
    config.rb_type               = pdcp_rb_type::drb;
    config.rlc_mode              = pdcp_rlc_mode::am;
    config.sn_size               = sn_size;
    config.direction             = pdcp_security_direction::downlink;
    config.out_of_order_delivery = false;
    config.t_reordering          = t_reordering;
    config.max_count             = max_count;

    // Set security keys
    sec_cfg.k_128_rrc_int = k_128_int;
    sec_cfg.k_128_up_int  = k_128_int;
    sec_cfg.k_128_rrc_enc = k_128_enc;
    sec_cfg.k_128_up_enc  = k_128_enc;

    // Set encription/integrity algorithms
    sec_cfg.integ_algo  = security::integrity_algorithm::nia1;
    sec_cfg.cipher_algo = security::ciphering_algorithm::nea1;

    // Create PDCP RX entity
    test_frame = std::make_unique<pdcp_rx_test_frame>();
    pdcp_rx    = std::make_unique<pdcp_entity_rx>(0, LCID_SRB1, config, *test_frame, *test_frame, timers);
    srslog::flush();
  }

  /// \brief Gets test PDU based on the COUNT and SN size
  /// \param count COUNT being tested
  /// \param exp_pdu Expected PDU that is set to the correct test vector
  void get_test_pdu(uint32_t count, byte_buffer& exp_pdu)
  {
    ASSERT_EQ(true, get_pdu_test_vector(sn_size, count, exp_pdu));
  }

  /// \brief Helper to advance the timers
  /// \param nof_tick Number of ticks to advance timers
  void tick_all(uint32_t nof_ticks)
  {
    for (uint32_t i = 0; i < nof_ticks; i++) {
      timers.tick_all();
    }
  }

  uint32_t SN(uint32_t count) const { return count & (0xffffffffU >> (32U - static_cast<uint8_t>(sn_size))); }
  uint32_t HFN(uint32_t count) const { return (count >> static_cast<uint8_t>(sn_size)); }
  uint32_t COUNT(uint32_t hfn, uint32_t sn) const { return (hfn << static_cast<uint8_t>(sn_size)) | sn; }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  pdcp_sn_size                        sn_size = {};
  pdcp_config::pdcp_rx_config         config  = {};
  timer_manager                       timers;
  std::unique_ptr<pdcp_rx_test_frame> test_frame = {};

  security::sec_128_as_config sec_cfg;

  std::unique_ptr<pdcp_entity_rx> pdcp_rx;
  pdcp_rx_lower_interface*        pdcp_rx_lower = nullptr;
};
} // namespace srsgnb
