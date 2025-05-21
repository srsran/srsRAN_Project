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

#include "../ngap_validators/ngap_validators.h"
#include "../ue_context/ngap_ue_context.h"
#include "metrics/ngap_metrics_aggregator.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_pdu_session_resource_setup_procedure
{
public:
  ngap_pdu_session_resource_setup_procedure(const cu_cp_pdu_session_resource_setup_request&    request_,
                                            const asn1::ngap::pdu_session_res_setup_request_s& asn1_request_,
                                            const ngap_ue_ids&                                 ue_ids_,
                                            ngap_cu_cp_notifier&                               cu_cp_notifier_,
                                            ngap_metrics_aggregator&                           metrics_handler_,
                                            ngap_message_notifier&                             amf_notif_,
                                            ngap_ue_logger&                                    logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Setup Procedure"; }

private:
  void combine_pdu_session_resource_setup_response();

  // Result senders.
  bool send_pdu_session_resource_setup_response();

  const cu_cp_pdu_session_resource_setup_request    request;
  const asn1::ngap::pdu_session_res_setup_request_s asn1_request;
  cu_cp_pdu_session_resource_setup_response         validation_response;
  byte_buffer                                       nas_pdu;
  const ngap_ue_ids                                 ue_ids;
  cu_cp_pdu_session_resource_setup_response         response;
  ngap_cu_cp_notifier&                              cu_cp_notifier;
  ngap_metrics_aggregator&                          metrics_handler;
  ngap_message_notifier&                            amf_notifier;
  ngap_ue_logger&                                   logger;

  cu_cp_ue_context_release_request ue_context_release_request;

  // Maps PDU session ID to S-NSSAI for metrics.
  std::map<pdu_session_id_t, s_nssai_t> pdu_session_id_to_snssai;

  // Procedure outcomes.
  pdu_session_resource_setup_validation_outcome verification_outcome;
};

} // namespace srs_cu_cp
} // namespace srsran
