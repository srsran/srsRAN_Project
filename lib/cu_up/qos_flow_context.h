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

#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
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
  };

  qos_flow_id_t qos_flow_id; // The QoS flow ID.
  uint16_t      five_qi;     // The FiveQI assigned to this flow.

  // TODO: add other fields contained in:
  // * qos_flow_level_qos_params_s
  // * ngran_alloc_and_retention_prio_s
  // * etc.

  std::unique_ptr<pdcp_tx_lower_notifier>      pdcp_tx_notifier;
  std::unique_ptr<pdcp_rx_upper_data_notifier> sdap_rx_notifier;

  // std::unique_ptr<f1u_message_notifier> rx_notifier      = std::make_unique<f1u_message_null_notifier>();
  // std::unique_ptr<rrc_pdu_notifier>     sdap_tx_notifier = std::make_unique<rrc_pdu_null_notifier>();
};

} // namespace srs_cu_up
} // namespace srsran
