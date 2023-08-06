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

#include "srsran/cu_cp/du_processor.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
/// TODO Add seqdiag
class pdu_session_resource_release_routine
{
public:
  pdu_session_resource_release_routine(const cu_cp_pdu_session_resource_release_command& release_cmd_,
                                       du_processor_e1ap_control_notifier&               e1ap_ctrl_notif_,
                                       du_processor_f1ap_ue_context_notifier&            f1ap_ue_ctxt_notif_,
                                       du_processor_ngap_control_notifier&               ngap_ctrl_notifier_,
                                       du_processor_ue_task_scheduler&                   task_sched_,
                                       up_resource_manager&                              rrc_ue_up_resource_manager_,
                                       srslog::basic_logger&                             logger_);

  void operator()(coro_context<async_task<cu_cp_pdu_session_resource_release_response>>& ctx);

  static const char* name() { return "PDU Session Resource Release Routine"; }

private:
  void fill_e1ap_bearer_context_modification_request(e1ap_bearer_context_modification_request& e1ap_request);
  bool valid_5qi(const qos_flow_setup_request_item& flow);

  cu_cp_pdu_session_resource_release_response generate_pdu_session_resource_release_response(bool success);

  async_task<void> request_context_release();

  const cu_cp_pdu_session_resource_release_command release_cmd;

  up_config_update next_config;

  du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier;         // to trigger bearer context setup at CU-UP
  du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notifier;      // to trigger UE context modification at DU
  du_processor_ngap_control_notifier&    ngap_ctrl_notifier;         // to request UE release
  du_processor_ue_task_scheduler&        task_sched;                 // to schedule UE release request (over NGAP)
  up_resource_manager&                   rrc_ue_up_resource_manager; // to get RRC DRB config
  srslog::basic_logger&                  logger;

  // (sub-)routine requests
  f1ap_ue_context_modification_request     ue_context_mod_request;
  e1ap_bearer_context_modification_request bearer_context_modification_request;

  // (sub-)routine results
  cu_cp_pdu_session_resource_release_response response_msg;
  f1ap_ue_context_modification_response       ue_context_modification_response; // to inform DU about the new DRBs
  e1ap_bearer_context_modification_response
      bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
};

} // namespace srs_cu_cp
} // namespace srsran
