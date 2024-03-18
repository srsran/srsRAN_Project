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

#include "adapters/pdcp_adapters.h"
#include "srsran/pdcp/pdcp_factory.h"

namespace srsran {
namespace srs_cu_cp {

struct pdcp_result {
  variant<byte_buffer, ngap_cause_t> result;

  /// Whether the packing/unpacking was successful.
  bool is_successful() const { return variant_holds_alternative<byte_buffer>(result); }

  ngap_cause_t get_failure_cause() const { return variant_get<ngap_cause_t>(result); }

  byte_buffer get_pdu() const { return byte_buffer{variant_get<byte_buffer>(result)}; }
};

/// Additional context of a SRB containing notifiers to PDCP, i.e. SRB1 and SRB2.
struct srb_pdcp_context {
  std::unique_ptr<pdcp_entity>   entity;
  pdcp_rrc_ue_tx_adapter         pdcp_tx_notifier;
  pdcp_tx_control_rrc_ue_adapter rrc_tx_control_notifier;
  pdcp_rrc_ue_rx_adapter         rrc_rx_data_notifier;
  pdcp_rx_control_rrc_ue_adapter rrc_rx_control_notifier;

  srb_pdcp_context(const ue_index_t ue_index, const srb_id_t srb_id, timer_factory timers)
  {
    // prepare PDCP creation message
    pdcp_entity_creation_message srb_pdcp{};
    srb_pdcp.ue_index    = ue_index_to_uint(ue_index);
    srb_pdcp.rb_id       = srb_id;
    srb_pdcp.config      = pdcp_make_default_srb_config(); // TODO: allow non-default PDCP SRB configs
    srb_pdcp.tx_lower    = &pdcp_tx_notifier;
    srb_pdcp.tx_upper_cn = &rrc_tx_control_notifier;
    srb_pdcp.rx_upper_dn = &rrc_rx_data_notifier;
    srb_pdcp.rx_upper_cn = &rrc_rx_control_notifier;
    // Uplink, Downlink and Control run in the same executor, hence all timer factories are the same.
    srb_pdcp.ue_dl_timer_factory   = timers;
    srb_pdcp.ue_ul_timer_factory   = timers;
    srb_pdcp.ue_ctrl_timer_factory = timers;

    // create PDCP entity
    entity = create_pdcp_entity(srb_pdcp);
  }
};

/// Context for a SRB.
class ue_srb_context
{
public:
  ue_srb_context(const ue_index_t ue_index, const srb_id_t srb_id, timer_factory timers) :
    pdcp_context(ue_index, srb_id, timers)
  {
  }

  // Setup security
  void enable_tx_security(security::integrity_enabled int_enabled,
                          security::ciphering_enabled ciph_enabled,
                          security::sec_128_as_config sec_cfg) const
  {
    // Configure tx security
    auto& pdcp_tx_ctrl = pdcp_context.entity->get_tx_upper_control_interface();
    pdcp_tx_ctrl.configure_security(sec_cfg);
    pdcp_tx_ctrl.set_integrity_protection(int_enabled);
    pdcp_tx_ctrl.set_ciphering(ciph_enabled);
  }

  void enable_rx_security(security::integrity_enabled int_enabled,
                          security::ciphering_enabled ciph_enabled,
                          security::sec_128_as_config sec_cfg) const
  {
    // Configure tx security
    auto& pdcp_rx_ctrl = pdcp_context.entity->get_rx_upper_control_interface();
    pdcp_rx_ctrl.configure_security(sec_cfg);
    pdcp_rx_ctrl.set_integrity_protection(int_enabled);
    pdcp_rx_ctrl.set_ciphering(ciph_enabled);
  }

  // Full security setup. Used e.g., with SRB2
  void enable_full_security(security::sec_128_as_config sec_cfg) const
  {
    // Configure rx security
    auto& pdcp_tx_ctrl = pdcp_context.entity->get_tx_upper_control_interface();
    pdcp_tx_ctrl.configure_security(sec_cfg);
    pdcp_tx_ctrl.set_integrity_protection(security::integrity_enabled::on);
    pdcp_tx_ctrl.set_ciphering(security::ciphering_enabled::on);

    // Configure tx security
    auto& pdcp_rx_ctrl = pdcp_context.entity->get_rx_upper_control_interface();
    pdcp_rx_ctrl.configure_security(sec_cfg);
    pdcp_rx_ctrl.set_integrity_protection(security::integrity_enabled::on);
    pdcp_rx_ctrl.set_ciphering(security::ciphering_enabled::on);
  }

  // Add ciphering and integrity protection to an RRC PDU.
  pdcp_result pack_rrc_pdu(byte_buffer rrc_pdu)
  {
    pdcp_context.entity->get_tx_upper_data_interface().handle_sdu(std::move(rrc_pdu));

    byte_buffer packed_pdu = pdcp_context.pdcp_tx_notifier.get_pdcp_pdu();

    // If the PDCP layer failed to pack the PDU, return the failure cause.
    if (packed_pdu.empty()) {
      return pdcp_result{pdcp_context.rrc_tx_control_notifier.get_failure_cause()};
    }

    return pdcp_result{std::move(packed_pdu)};
  }

  // Decipher and verify integrity of an PDCP PDU.
  pdcp_result unpack_pdcp_pdu(byte_buffer pdcp_pdu)
  {
    auto buffer_chain = byte_buffer_chain::create(std::move(pdcp_pdu));
    if (buffer_chain.is_error()) {
      return pdcp_result{pdcp_context.rrc_rx_control_notifier.get_failure_cause()};
    }

    pdcp_context.entity->get_rx_lower_interface().handle_pdu(std::move(buffer_chain.value()));

    byte_buffer unpacked_pdu = pdcp_context.rrc_rx_data_notifier.get_rrc_pdu();

    // If the PDCP layer failed to unpack the PDU, return the failure cause.
    if (unpacked_pdu.empty()) {
      return pdcp_result{pdcp_context.rrc_rx_control_notifier.get_failure_cause()};
    }

    return pdcp_result{std::move(unpacked_pdu)};
  }

private:
  srb_pdcp_context pdcp_context;
};

} // namespace srs_cu_cp
} // namespace srsran
