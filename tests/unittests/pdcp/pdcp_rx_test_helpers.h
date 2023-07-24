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

#pragma once

#include "lib/pdcp/pdcp_entity_rx.h"
#include "lib/pdcp/pdcp_interconnect.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsran {

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_rx_test_frame : public pdcp_tx_status_handler,
                           public pdcp_rx_upper_data_notifier,
                           public pdcp_rx_upper_control_notifier
{
public:
  std::queue<byte_buffer>       sdu_queue              = {};
  uint32_t                      sdu_counter            = 0;
  uint32_t                      integrity_fail_counter = 0;
  uint32_t                      nof_max_count_reached  = 0;
  uint32_t                      nof_protocol_failure   = 0;
  std::queue<byte_buffer_chain> status_report_queue    = {};

  /// PDCP TX status handler
  void on_status_report(byte_buffer_chain status) override { status_report_queue.push(std::move(status)); }

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
            pdcp_rb_type      rb_type_     = pdcp_rb_type::drb,
            pdcp_rlc_mode     rlc_mode_    = pdcp_rlc_mode::am,
            pdcp_t_reordering t_reordering = pdcp_t_reordering::ms10,
            pdcp_max_count    max_count    = {pdcp_rx_default_max_count_notify, pdcp_rx_default_max_count_hard})
  {
    logger.info("Creating PDCP RX ({} bit)", pdcp_sn_size_to_uint(sn_size_));

    sn_size = sn_size_;

    // Set Rx config
    config.rb_type               = rb_type_;
    config.rlc_mode              = rlc_mode_;
    config.sn_size               = sn_size;
    config.direction             = pdcp_security_direction::downlink;
    config.out_of_order_delivery = false;
    config.t_reordering          = t_reordering;
    config.max_count             = max_count;

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
    sec_cfg.integ_algo  = security::integrity_algorithm::nia1;
    sec_cfg.cipher_algo = security::ciphering_algorithm::nea1;

    // Create PDCP RX entity
    test_frame = std::make_unique<pdcp_rx_test_frame>();
    pdcp_rx =
        std::make_unique<pdcp_entity_rx>(0, rb_id, config, *test_frame, *test_frame, timer_factory{timers, worker});
    pdcp_rx->set_status_handler(test_frame.get());

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
      timers.tick();
      worker.run_pending_tasks();
    }
  }

  uint32_t SN(uint32_t count) const { return count & (0xffffffffU >> (32U - static_cast<uint8_t>(sn_size))); }
  uint32_t HFN(uint32_t count) const { return (count >> static_cast<uint8_t>(sn_size)); }
  uint32_t COUNT(uint32_t hfn, uint32_t sn) const { return (hfn << static_cast<uint8_t>(sn_size)) | sn; }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  pdcp_sn_size                        sn_size = {};
  pdcp_rx_config                      config  = {};
  timer_manager                       timers;
  manual_task_worker                  worker{64};
  std::unique_ptr<pdcp_rx_test_frame> test_frame = {};

  security::sec_128_as_config sec_cfg;

  std::unique_ptr<pdcp_entity_rx> pdcp_rx;
  pdcp_rx_lower_interface*        pdcp_rx_lower = nullptr;
};
} // namespace srsran
