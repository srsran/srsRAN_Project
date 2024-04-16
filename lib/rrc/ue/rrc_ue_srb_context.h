/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adapters/pdcp_adapters.h"
#include "srsran/pdcp/pdcp_factory.h"

namespace srsran {
namespace srs_cu_cp {

struct pdcp_result {
  variant<std::vector<byte_buffer>, ngap_cause_t> result;

  /// Whether the packing/unpacking was successful.
  bool is_successful() const { return variant_holds_alternative<std::vector<byte_buffer>>(result); }

  ngap_cause_t get_failure_cause() const { return variant_get<ngap_cause_t>(result); }

  std::vector<byte_buffer> pop_pdus() { return std::move(variant_get<std::vector<byte_buffer>>(result)); }
};

/// Additional context of a SRB containing notifiers to PDCP, i.e. SRB1 and SRB2.
struct srb_pdcp_context {
  std::unique_ptr<pdcp_entity>   entity;
  pdcp_rrc_ue_tx_adapter         pdcp_tx_notifier;
  pdcp_tx_control_rrc_ue_adapter rrc_tx_control_notifier;
  pdcp_rrc_ue_rx_adapter         rrc_rx_data_notifier;
  pdcp_rx_control_rrc_ue_adapter rrc_rx_control_notifier;

  srb_pdcp_context(const ue_index_t ue_index, const srb_id_t srb_id, timer_factory timers, task_executor& executor)
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
    // Uplink, Downlink, Control and Crypto run in the same executor
    srb_pdcp.ue_dl_executor  = &executor;
    srb_pdcp.ue_ul_executor  = &executor;
    srb_pdcp.crypto_executor = &executor;

    // create PDCP entity
    entity = create_pdcp_entity(srb_pdcp);
  }
};

/// Context for a SRB.
class ue_srb_context
{
public:
  ue_srb_context(const ue_index_t ue_index, const srb_id_t srb_id, timer_factory timers, task_executor& executor) :
    pdcp_context(ue_index, srb_id, timers, executor)
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

    return pdcp_result{std::vector<byte_buffer>{std::move(packed_pdu)}};
  }

  // Decipher and verify integrity of an PDCP PDU.
  pdcp_result unpack_pdcp_pdu(byte_buffer pdcp_pdu)
  {
    auto buffer_chain = byte_buffer_chain::create(std::move(pdcp_pdu));
    if (buffer_chain.is_error()) {
      return pdcp_result{ngap_cause_misc_t::not_enough_user_plane_processing_res};
    }

    pdcp_context.entity->get_rx_lower_interface().handle_pdu(std::move(buffer_chain.value()));

    std::vector<byte_buffer> unpacked_pdus = pdcp_context.rrc_rx_data_notifier.pop_rrc_pdus();

    // If the PDCP layer failed to unpack the PDU, return the failure cause.
    if (unpacked_pdus.empty()) {
      optional<ngap_cause_t> cause = pdcp_context.rrc_rx_control_notifier.pop_failure_cause();
      if (cause.has_value()) {
        // There was an error in the PDCP unpacking.
        return pdcp_result{cause.value()};
      }
      // The PDCP PDU was buffered because it was out of order.
    }

    return pdcp_result{std::move(unpacked_pdus)};
  }

private:
  srb_pdcp_context pdcp_context;
};

} // namespace srs_cu_cp
} // namespace srsran
