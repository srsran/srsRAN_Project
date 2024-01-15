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
    if (qos_params.non_dyn_5qi.has_value()) {
      five_qi = qos_params.non_dyn_5qi.value().five_qi;
    }
    srsran_assert(qos_params.dyn_5qi.has_value() == false, "Dynamic 5QI not supported.");
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
