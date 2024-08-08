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

#include "adapters/sdap_adapters.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/ran/cu_types.h"

namespace srsran {
namespace srs_cu_up {

struct qos_flow_context {
  qos_flow_context(const e1ap_qos_flow_qos_param_item& flow) : qos_flow_id(flow.qos_flow_id)
  {
    const auto& qos_params = flow.qos_flow_level_qos_params.qos_characteristics;
    five_qi                = qos_params.get_5qi();
    srsran_assert(not qos_params.is_dyn_5qi(), "Dynamic 5QI not supported.");
    srsran_assert(five_qi != five_qi_t::invalid, "FiveQI must be set.");
  };

  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid; // The QoS flow ID.
  five_qi_t     five_qi     = five_qi_t::invalid;     // The FiveQI assigned to this flow.

  // TODO: add other fields contained in:
  // * qos_flow_level_qos_params_s
  // * ngran_alloc_and_retention_prio_s
  // * etc.

  sdap_pdcp_adapter sdap_to_pdcp_adapter;

  std::unique_ptr<pdcp_rx_upper_data_notifier> sdap_rx_notifier;
};

} // namespace srs_cu_up
} // namespace srsran
