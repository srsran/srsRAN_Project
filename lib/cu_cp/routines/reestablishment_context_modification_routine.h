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
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the modification of an existing PDU session resources.
/// TODO Add seqdiag
class reestablishment_context_modification_routine
{
public:
  reestablishment_context_modification_routine(ue_index_t                                    ue_index_,
                                               const srsran::security::sec_as_config&        security_cfg_,
                                               e1ap_bearer_context_manager&                  e1ap_bearer_ctxt_mng_,
                                               f1ap_ue_context_manager&                      f1ap_ue_ctxt_mng_,
                                               du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_,
                                               cu_cp_rrc_ue_interface&                       cu_cp_notifier_,
                                               ue_task_scheduler&                            ue_task_sched_,
                                               up_resource_manager&                          up_resource_mng_,
                                               srslog::basic_logger&                         logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "Reestablishment Context Modification Routine"; }

private:
  bool generate_bearer_context_modification_request_for_new_ul_tnl();
  bool generate_ue_context_modification_request(
      f1ap_ue_context_modification_request& ue_context_mod_req,
      const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>&
          e1ap_pdu_session_resource_modify_list);

  bool generate_bearer_context_modification(e1ap_bearer_context_modification_request&        bearer_ctxt_mod_req,
                                            const e1ap_bearer_context_modification_response& bearer_ctxt_mod_resp,
                                            const f1ap_ue_context_modification_response& ue_context_modification_resp,
                                            up_resource_manager&                         up_resource_manager,
                                            bool                                         reestablish_pdcp);

  ue_index_t                                    ue_index = ue_index_t::invalid;
  security::sec_as_config                       security_cfg;
  e1ap_bearer_context_manager&                  e1ap_bearer_ctxt_mng; // to trigger bearer context setup at CU-UP
  f1ap_ue_context_manager&                      f1ap_ue_ctxt_mng;     // to trigger UE context modification at DU
  du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier;      // to trigger RRC Reconfiguration at UE
  cu_cp_rrc_ue_interface&                       cu_cp_notifier;       // to trigger UE release at CU-CP
  ue_task_scheduler&                            ue_task_sched;        // to schedule UE release request
  up_resource_manager&                          up_resource_mng;      // to get RRC DRB config
  srslog::basic_logger&                         logger;

  // (sub-)routine requests
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  f1ap_ue_context_modification_request     ue_context_mod_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;

  // (sub-)routine results
  cu_cp_pdu_session_resource_modify_response response_msg;                     // Final routine result.
  f1ap_ue_context_modification_response      ue_context_modification_response; // to inform DU about the new DRBs
  e1ap_bearer_context_modification_response
       bearer_context_modification_response; // to inform CU-UP about the new TEID for UL F1u traffic
  bool rrc_reconfig_result = false;          // the final UE reconfiguration
};

} // namespace srs_cu_cp
} // namespace srsran
