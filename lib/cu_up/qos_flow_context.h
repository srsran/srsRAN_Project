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

#include "srsgnb/e1ap/common/e1ap_types.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/ran/cu_types.h"

namespace srsgnb {
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
} // namespace srsgnb
