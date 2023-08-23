/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adapters/pdcp_adapters.h"

namespace srsran {
namespace srs_cu_cp {

/// Additional context of a SRB containing notifiers to PDCP, i.e. SRB1 and SRB2.
struct srb_pdcp_context {
  std::unique_ptr<pdcp_entity>                    entity;
  std::unique_ptr<pdcp_rrc_ue_tx_adapter>         pdcp_tx_notifier;
  std::unique_ptr<pdcp_tx_control_rrc_ue_adapter> rrc_tx_control_notifier;
  std::unique_ptr<pdcp_rrc_ue_rx_adapter>         rrc_rx_data_notifier;
  std::unique_ptr<pdcp_rx_control_rrc_ue_adapter> rrc_rx_control_notifier;
};

/// Context for a SRB.
struct ue_srb_context {
  // Setup security
  void enable_security(security::sec_128_as_config sec_cfg)
  {
    pdcp_context.entity->get_rx_upper_control_interface().enable_security(sec_cfg);
    pdcp_context.entity->get_tx_upper_control_interface().enable_security(sec_cfg);
  }

  // Add ciphering and integrity protection to an RRC PDU.
  byte_buffer pack_rrc_pdu(byte_buffer rrc_pdu)
  {
    pdcp_context.entity->get_tx_upper_data_interface().handle_sdu(std::move(rrc_pdu));
    return pdcp_context.pdcp_tx_notifier->get_pdcp_pdu();
  }

  // Decipher and verify integrity of an PDCP PDU.
  byte_buffer unpack_pdcp_pdu(byte_buffer pdcp_pdu)
  {
    pdcp_context.entity->get_rx_lower_interface().handle_pdu(byte_buffer_chain{std::move(pdcp_pdu)});
    return pdcp_context.rrc_rx_data_notifier->get_rrc_pdu();
  }

  srb_pdcp_context pdcp_context;
};

} // namespace srs_cu_cp
} // namespace srsran
