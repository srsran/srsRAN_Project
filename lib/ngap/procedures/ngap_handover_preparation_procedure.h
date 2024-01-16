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

#include "../ngap_context.h"
#include "../ue_context/ngap_ue_context.h"
#include "ngap_transaction_manager.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_handover_preparation_procedure
{
public:
  ngap_handover_preparation_procedure(const ngap_handover_preparation_request& req_,
                                      const ngap_context_t&                    context_,
                                      const ngap_ue_ids&                       ue_ids_,
                                      ngap_message_notifier&                   amf_notif_,
                                      ngap_rrc_ue_control_notifier&            rrc_ue_notif_,
                                      up_resource_manager&                     up_manager_,
                                      ngap_transaction_manager&                ev_mng_,
                                      timer_factory                            timers,
                                      ngap_ue_logger&                          logger_);

  void operator()(coro_context<async_task<ngap_handover_preparation_response>>& ctx);

  static const char* name() { return "Handover Preparation Procedure"; }

private:
  const ngap_handover_preparation_request request;
  const ngap_context_t                    context;
  const ngap_ue_ids                       ue_ids;
  ngap_message_notifier&                  amf_notifier;
  ngap_rrc_ue_control_notifier&           rrc_ue_notifier;
  up_resource_manager&                    up_manager;
  ngap_transaction_manager&               ev_mng;
  ngap_ue_logger&                         logger;

  unique_timer tng_reloc_prep_timer;

  ngap_ue_source_handover_context ho_ue_context;

  protocol_transaction_outcome_observer<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> transaction_sink;

  void get_required_handover_context();
  void send_handover_required();
  bool forward_rrc_handover_command();

  // ASN.1 helpers
  void        fill_asn1_target_ran_node_id(asn1::ngap::target_id_c& target_id);
  void        fill_asn1_pdu_session_res_list(asn1::ngap::pdu_session_res_list_ho_rqd_l& pdu_session_res_list);
  byte_buffer fill_asn1_source_to_target_transparent_container();

  // TNG Reloc Prep timeout for Handover Preparation procedure.
  // Handover cancellation procedure will be initialized if timer fires.
  // FIXME: Set to appropriate value
  const std::chrono::milliseconds tng_reloc_prep_ms{1000};
};

} // namespace srs_cu_cp
} // namespace srsran
