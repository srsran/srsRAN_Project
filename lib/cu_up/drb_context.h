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

#include "adapters/f1u_adapters.h"
#include "adapters/pdcp_adapters.h"
#include "qos_flow_context.h"
#include "srsgnb/pdcp/pdcp_entity.h"
#include "srsgnb/ran/lcid.h"
#include <map>

namespace srsgnb {
namespace srs_cu_up {

/// \brief DRB context with map to all QoS flows.
struct drb_context {
  drb_context(const drb_id_t& drb_id_) : drb_id(drb_id_){};

  drb_id_t drb_id;
  uint32_t f1u_ul_teid;

  std::unique_ptr<f1u_bearer>  f1u;
  std::unique_ptr<pdcp_entity> pdcp;

  // Adapter PDCP->SDAP
  // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
  pdcp_sdap_adapter pdcp_to_sdap_adapter;
  pdcp_f1u_adapter  pdcp_to_f1u_adapter;
  f1u_pdcp_adapter  f1u_to_pdcp_adapter;

  // Adapter PDCP->E1
  pdcp_rx_e1_adapter pdcp_rx_to_e1_adapter;
  pdcp_tx_e1_adapter pdcp_tx_to_e1_adapter;

  uint8_t cell_group_id; /// This can/should be a list of cell groups.

  std::map<qos_flow_id_t, std::unique_ptr<qos_flow_context>> qos_flows; // key is qos_flow_id
};

} // namespace srs_cu_up
} // namespace srsgnb
