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

#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/pdcp/pdcp_rx.h"
#include "srsgnb/pdcp/pdcp_tx.h"

namespace srsgnb {
namespace srs_cu_up {

struct qos_flow_context {
  qos_flow_context(const asn1::e1ap::qo_s_flow_qos_param_item_s& flow) : qos_flow_id(flow.qo_s_flow_id)
  {
    const auto& qos_params = flow.qo_sflow_level_qos_params.qo_s_characteristics;
    if (qos_params.type() == asn1::e1ap::qo_s_characteristics_c::types_opts::non_dynamic_minus5_qi) {
      five_qi = qos_params.non_dynamic_minus5_qi().five_qi;
    }
  };

  uint8_t  qos_flow_id; // The QoS flow ID.
  uint16_t five_qi;     // The FiveQI assigned to this flow.

  // TODO: add other fields contained in:
  // * qo_sflow_level_qos_params_s
  // * ngran_alloc_and_retention_prio_s
  // * etc.

  std::unique_ptr<pdcp_tx_lower_notifier>      pdcp_tx_notifier;
  std::unique_ptr<pdcp_rx_upper_data_notifier> sdap_rx_notifier;

  // std::unique_ptr<f1u_message_notifier> rx_notifier      = std::make_unique<f1u_message_null_notifier>();
  // std::unique_ptr<rrc_pdu_notifier>     sdap_tx_notifier = std::make_unique<rrc_pdu_null_notifier>();
};

} // namespace srs_cu_up
} // namespace srsgnb
