/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "lib/pdcp/pdcp_entity_tx.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsran {

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_tx_test_frame : public pdcp_rx_status_provider,
                           public pdcp_tx_lower_notifier,
                           public pdcp_tx_upper_control_notifier
{
public:
  std::queue<pdcp_tx_pdu> pdu_queue             = {};
  uint32_t                nof_max_count_reached = 0;
  uint32_t                nof_protocol_failure  = 0;
  std::queue<uint32_t>    sdu_discard_queue     = {};

  // PDCP RX status provider
  byte_buffer compile_status_report() final
  {
    // Build status report dummy to be picked by the TX entity
    byte_buffer buf = {};
    bit_encoder enc(buf);

    // Pack PDU header
    enc.pack(to_number(pdcp_dc_field::control), 1);
    enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
    enc.pack(0b0000, 4);

    // Pack something into FMC field
    enc.pack(0xc0cac01a, 32);

    // Pack some bitmap
    enc.pack(0xcafe, 16);

    return buf;
  }

  // PDCP TX upper layer control notifier
  void on_max_count_reached() final { nof_max_count_reached++; }
  void on_protocol_failure() final { nof_protocol_failure++; }

  // PDCP TX lower layer data notifier
  void on_new_pdu(pdcp_tx_pdu pdu) final { pdu_queue.push(std::move(pdu)); }
  void on_discard_pdu(uint32_t pdcp_sn) final { sdu_discard_queue.push(pdcp_sn); }
};

/// Fixture class for PDCP tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_tx_test_helper
{
public:
  virtual ~pdcp_tx_test_helper() = default;

protected:
  /// Virtual function to be called just before creating the PDCP entity.
  /// This function provides an interface for config adjustments of selected testcases with different PDCP config.
  virtual void init_adjustments() {}

  /// \brief Initializes fixture according to size sequence number size
  /// \param sn_size_ size of the sequence number
  void init(std::tuple<pdcp_sn_size, unsigned> cfg_tuple,
            pdcp_rb_type                       rb_type_      = pdcp_rb_type::drb,
            pdcp_rlc_mode                      rlc_mode_     = pdcp_rlc_mode::am,
            pdcp_discard_timer                 discard_timer = pdcp_discard_timer::ms10,
            pdcp_max_count max_count = {pdcp_tx_default_max_count_notify, pdcp_tx_default_max_count_hard})
  {
    logger.info("Creating PDCP TX ({} bit, NIA{}, NEA{})",
                pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(cfg_tuple)),
                std::get<unsigned>(cfg_tuple),
                std::get<unsigned>(cfg_tuple));

    sn_size     = std::get<pdcp_sn_size>(cfg_tuple);
    algo        = std::get<unsigned>(cfg_tuple);
    pdu_hdr_len = pdcp_data_pdu_header_size(sn_size); // Round up division

    // Set TX config
    config.sn_size                = sn_size;
    config.rb_type                = rb_type_;
    config.rlc_mode               = rlc_mode_;
    config.direction              = pdcp_security_direction::downlink;
    config.discard_timer          = discard_timer;
    config.custom.max_count       = max_count;
    config.status_report_required = true;

    // RB_id and security domain
    rb_id_t rb_id;
    switch (rb_type_) {
      case pdcp_rb_type::srb:
        sec_cfg.domain = security::sec_domain::rrc;
        rb_id          = srb_id_t::srb1;
        break;
      case pdcp_rb_type::drb:
        sec_cfg.domain = security::sec_domain::up;
        rb_id          = drb_id_t::drb1;
        break;
    }

    // Set security keys
    sec_cfg.k_128_int = k_128_int;
    sec_cfg.k_128_enc = k_128_enc;

    // Set encription/integrity algorithms
    sec_cfg.integ_algo  = static_cast<security::integrity_algorithm>(std::get<unsigned>(cfg_tuple));
    sec_cfg.cipher_algo = static_cast<security::ciphering_algorithm>(std::get<unsigned>(cfg_tuple));

    // Allow for config adjustments
    init_adjustments();

    // Create PDCP entity
    pdcp_tx = std::make_unique<pdcp_entity_tx>(0, rb_id, config, test_frame, test_frame, timer_factory{timers, worker});
    pdcp_tx->set_status_provider(&test_frame);
  }

  /// \brief Gets expected PDU based on the COUNT being tested
  /// \param count COUNT being tested
  /// \param exp_pdu Expected PDU that is set to the correct test vector
  void get_expected_pdu(uint32_t count, byte_buffer& exp_pdu)
  {
    ASSERT_EQ(true, get_pdu_test_vector(sn_size, count, exp_pdu, algo));
  }

  /// \brief Helper to advance the timers
  /// \param nof_tick Number of ticks to advance timers
  void tick_all(uint32_t nof_ticks)
  {
    for (uint32_t i = 0; i < nof_ticks; i++) {
      timers.tick();
      worker.run_pending_tasks();
    }
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  pdcp_sn_size       sn_size = {};
  unsigned           algo    = {};
  uint32_t           pdu_hdr_len;
  uint32_t           mac_hdr_len = 4;
  pdcp_tx_config     config      = {};
  timer_manager      timers;
  manual_task_worker worker{4098};
  pdcp_tx_test_frame test_frame = {};

  // Security configuration
  security::sec_128_as_config sec_cfg = {};

  // Default max COUNT
  const pdcp_max_count default_max_count = {pdcp_tx_default_max_count_notify, pdcp_tx_default_max_count_hard};

  std::unique_ptr<pdcp_entity_tx> pdcp_tx;
};
} // namespace srsran
