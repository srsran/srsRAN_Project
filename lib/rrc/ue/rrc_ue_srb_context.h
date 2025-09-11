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

#pragma once

#include "adapters/pdcp_adapters.h"
#include "srsran/pdcp/pdcp_factory.h"
#include "srsran/support/executors/inline_task_executor.h"

namespace srsran {
namespace srs_cu_cp {

struct pdcp_tx_result {
  std::variant<byte_buffer, ngap_cause_t> result;

  /// Whether the packing was successful.
  bool is_successful() const { return std::holds_alternative<byte_buffer>(result); }

  ngap_cause_t get_failure_cause() const { return std::get<ngap_cause_t>(result); }

  byte_buffer pop_pdu() { return std::move(std::get<byte_buffer>(result)); }
};

struct pdcp_rx_result {
  std::variant<std::vector<byte_buffer>, ngap_cause_t> result;

  /// Whether the unpacking was successful.
  bool is_successful() const { return std::holds_alternative<std::vector<byte_buffer>>(result); }

  ngap_cause_t get_failure_cause() const { return std::get<ngap_cause_t>(result); }

  std::vector<byte_buffer> pop_pdus() { return std::move(std::get<std::vector<byte_buffer>>(result)); }
};

/// Additional context of a SRB containing notifiers to PDCP, i.e. SRB1 and SRB2.
struct srb_pdcp_context {
  inline_task_executor           inline_executor; // TODO: refactor F1AP msg handling to support async PDCP execution
  std::unique_ptr<pdcp_entity>   entity;
  pdcp_rrc_ue_tx_adapter         pdcp_tx_notifier;
  pdcp_tx_control_rrc_ue_adapter rrc_tx_control_notifier;
  pdcp_rrc_ue_rx_adapter         rrc_rx_data_notifier;

  srb_pdcp_context(const ue_index_t ue_index,
                   const srb_id_t   srb_id,
                   timer_factory    timers,
                   task_executor&   executor,
                   uint32_t         max_nof_crypto_workers)
  {
    // prepare PDCP creation message
    pdcp_entity_creation_message srb_pdcp{};
    srb_pdcp.ue_index    = ue_index_to_uint(ue_index);
    srb_pdcp.rb_id       = srb_id;
    srb_pdcp.config      = pdcp_make_default_srb_config(); // TODO: allow non-default PDCP SRB configs
    srb_pdcp.tx_lower    = &pdcp_tx_notifier;
    srb_pdcp.tx_upper_cn = &rrc_tx_control_notifier;
    srb_pdcp.rx_upper_dn = &rrc_rx_data_notifier;
    srb_pdcp.rx_upper_cn = &rrc_rx_data_notifier;
    // Uplink, Downlink and Control run in the same executor, hence all timer factories are the same.
    srb_pdcp.ue_dl_timer_factory   = timers;
    srb_pdcp.ue_ul_timer_factory   = timers;
    srb_pdcp.ue_ctrl_timer_factory = timers;
    // Uplink, Downlink, Control and Crypto run in the same executor
    srb_pdcp.ue_dl_executor         = &inline_executor;
    srb_pdcp.ue_ul_executor         = &inline_executor; // synchronous inline execution required to unpack RRC msgs
    srb_pdcp.ue_ctrl_executor       = &executor;
    srb_pdcp.crypto_executor        = &inline_executor; // synchronous inline execution required to unpack RRC msgs
    srb_pdcp.max_nof_crypto_workers = max_nof_crypto_workers;

    // create PDCP entity
    entity = create_pdcp_entity(srb_pdcp);
  }
};

/// Context for a SRB.
class ue_srb_context
{
public:
  ue_srb_context(const ue_index_t ue_index,
                 const srb_id_t   srb_id,
                 timer_factory    timers,
                 task_executor&   executor,
                 uint32_t         max_nof_crypto_workers) :
    pdcp_context(ue_index, srb_id, timers, executor, max_nof_crypto_workers)
  {
  }

  // Setup security
  void enable_tx_security(security::integrity_enabled int_enabled,
                          security::ciphering_enabled ciph_enabled,
                          security::sec_128_as_config sec_cfg) const
  {
    // Configure tx security
    auto& pdcp_tx_ctrl = pdcp_context.entity->get_tx_upper_control_interface();
    pdcp_tx_ctrl.configure_security(sec_cfg, int_enabled, ciph_enabled);
  }

  void enable_rx_security(security::integrity_enabled int_enabled,
                          security::ciphering_enabled ciph_enabled,
                          security::sec_128_as_config sec_cfg) const
  {
    // Configure rx security
    auto& pdcp_rx_ctrl = pdcp_context.entity->get_rx_upper_control_interface();
    pdcp_rx_ctrl.configure_security(sec_cfg, int_enabled, ciph_enabled);
  }

  // Full security setup. Used e.g., with SRB2
  void enable_full_security(security::sec_128_as_config sec_cfg) const
  {
    // Configure rx security
    auto& pdcp_tx_ctrl = pdcp_context.entity->get_tx_upper_control_interface();
    pdcp_tx_ctrl.configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    // Configure tx security
    auto& pdcp_rx_ctrl = pdcp_context.entity->get_rx_upper_control_interface();
    pdcp_rx_ctrl.configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);
  }

  // Add ciphering and integrity protection to an RRC PDU.
  pdcp_tx_result pack_rrc_pdu(byte_buffer rrc_pdu)
  {
    pdcp_context.entity->get_tx_upper_data_interface().handle_sdu(std::move(rrc_pdu));

    byte_buffer packed_pdu = pdcp_context.pdcp_tx_notifier.get_pdcp_pdu();

    // If the PDCP layer failed to pack the PDU, return the failure cause.
    if (packed_pdu.empty()) {
      return pdcp_tx_result{pdcp_context.rrc_tx_control_notifier.get_failure_cause()};
    }

    return pdcp_tx_result{std::move(packed_pdu)};
  }

  // Decipher and verify integrity of an PDCP PDU.
  pdcp_rx_result unpack_pdcp_pdu(byte_buffer pdcp_pdu)
  {
    auto buffer_chain = byte_buffer_chain::create(std::move(pdcp_pdu));
    if (not buffer_chain.has_value()) {
      return pdcp_rx_result{ngap_cause_misc_t::not_enough_user_plane_processing_res};
    }

    pdcp_context.entity->get_rx_lower_interface().handle_pdu(std::move(buffer_chain.value()));

    // Return unpacked PDCP PDUs or error with cause.
    // Note: List of byte_buffers (in case of success) can be empty if the PDCP Rx PDU is out-of-order.
    std::variant<std::vector<byte_buffer>, ngap_cause_t> unpacked_pdus = pdcp_context.rrc_rx_data_notifier.pop_result();
    return pdcp_rx_result{unpacked_pdus};
  }

private:
  srb_pdcp_context pdcp_context;
};

} // namespace srs_cu_cp
} // namespace srsran
