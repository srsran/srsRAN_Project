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

#include "../cu_cp_impl_interface.h"
#include "../du_processor/du_processor.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "srsran/cu_cp/ue_task_scheduler.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of PDU session resources from the RRC viewpoint.
/// TODO Add seqdiag
class pdu_session_resource_release_routine
{
public:
  pdu_session_resource_release_routine(const cu_cp_pdu_session_resource_release_command& release_cmd_,
                                       e1ap_bearer_context_manager&                      e1ap_bearer_ctxt_mng_,
                                       f1ap_ue_context_manager&                          f1ap_ue_ctxt_mng_,
                                       du_processor_rrc_ue_control_message_notifier&     rrc_ue_notifier_,
                                       cu_cp_rrc_ue_interface&                           cu_cp_notifier_,
                                       ue_task_scheduler&                                task_sched_,
                                       up_resource_manager&                              up_resource_mng_,
                                       srslog::basic_logger&                             logger_);

  void operator()(coro_context<async_task<cu_cp_pdu_session_resource_release_response>>& ctx);

  static const char* name() { return "PDU Session Resource Release Routine"; }

private:
  void fill_e1ap_bearer_context_modification_request(e1ap_bearer_context_modification_request& e1ap_request);
  bool valid_5qi(const qos_flow_setup_request_item& flow);

  cu_cp_pdu_session_resource_release_response handle_pdu_session_resource_release_response(bool success);

  const cu_cp_pdu_session_resource_release_command release_cmd;

  up_config_update next_config;

  e1ap_bearer_context_manager&                  e1ap_bearer_ctxt_mng; // to trigger bearer context setup at CU-UP
  f1ap_ue_context_manager&                      f1ap_ue_ctxt_mng;     // to trigger UE context modification at DU
  du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier;      // to trigger RRC Reconfiguration at UE
  cu_cp_rrc_ue_interface&                       cu_cp_notifier;       // to trigger UE release at CU-CP
  ue_task_scheduler&                            task_sched;           // to schedule UE release request
  up_resource_manager&                          up_resource_mng;      // to get RRC DRB config
  srslog::basic_logger&                         logger;

  // (sub-)routine requests
  f1ap_ue_context_modification_request     ue_context_mod_request;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  e1ap_bearer_context_release_command      bearer_context_release_command;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;
  cu_cp_ue_context_release_request         ue_context_release_request;

  // (sub-)routine results
  cu_cp_pdu_session_resource_release_response response_msg;
  f1ap_ue_context_modification_response       ue_context_modification_response; // to inform DU about the new DRBs
  e1ap_bearer_context_modification_response
       bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
  bool rrc_reconfig_result = false;          // the final UE reconfiguration
};

} // namespace srs_cu_cp
} // namespace srsran
