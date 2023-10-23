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

#include "du_processor_routine_manager.h"
#include "../routines/mobility/inter_cu_handover_target_routine.h"
#include "../routines/mobility/inter_du_handover_routine.h"
#include "../routines/mobility/inter_ngran_node_n2_handover_routine.h"
#include "../routines/pdu_session_resource_modification_routine.h"
#include "../routines/pdu_session_resource_release_routine.h"
#include "../routines/pdu_session_resource_setup_routine.h"
#include "../routines/reestablishment_context_modification_routine.h"
#include "../routines/ue_context_release_routine.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

du_processor_routine_manager::du_processor_routine_manager(
    du_processor_e1ap_control_notifier&    e1ap_ctrl_notifier_,
    du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notifier_,
    du_processor_ue_manager&               ue_manager_,
    const security_indication_t&           default_security_indication_,
    srslog::basic_logger&                  logger_) :
  e1ap_ctrl_notifier(e1ap_ctrl_notifier_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notifier_),
  ue_manager(ue_manager_),
  default_security_indication(default_security_indication_),
  logger(logger_)
{
}

async_task<cu_cp_pdu_session_resource_setup_response>
du_processor_routine_manager::start_pdu_session_resource_setup_routine(
    const cu_cp_pdu_session_resource_setup_request& setup_msg,
    const srsran::security::sec_as_config&          security_cfg,
    du_processor_rrc_ue_control_message_notifier&   rrc_ue_ctrl_notifier,
    up_resource_manager&                            rrc_ue_up_resource_manager)
{
  return launch_async<pdu_session_resource_setup_routine>(setup_msg,
                                                          ue_manager.get_ue_config(),
                                                          security_cfg,
                                                          default_security_indication,
                                                          e1ap_ctrl_notifier,
                                                          f1ap_ue_ctxt_notifier,
                                                          rrc_ue_ctrl_notifier,
                                                          rrc_ue_up_resource_manager,
                                                          logger);
}

async_task<cu_cp_pdu_session_resource_modify_response>
du_processor_routine_manager::start_pdu_session_resource_modification_routine(
    const cu_cp_pdu_session_resource_modify_request& modify_msg,
    du_processor_rrc_ue_control_message_notifier&    rrc_ue_ctrl_notifier,
    up_resource_manager&                             rrc_ue_up_resource_manager)
{
  return launch_async<pdu_session_resource_modification_routine>(
      modify_msg, e1ap_ctrl_notifier, f1ap_ue_ctxt_notifier, rrc_ue_ctrl_notifier, rrc_ue_up_resource_manager, logger);
}

async_task<cu_cp_pdu_session_resource_release_response>
du_processor_routine_manager::start_pdu_session_resource_release_routine(
    const cu_cp_pdu_session_resource_release_command& release_cmd,
    du_processor_ngap_control_notifier&               ngap_ctrl_notifier,
    du_processor_ue_task_scheduler&                   task_sched,
    up_resource_manager&                              rrc_ue_up_resource_manager)
{
  return launch_async<pdu_session_resource_release_routine>(release_cmd,
                                                            e1ap_ctrl_notifier,
                                                            f1ap_ue_ctxt_notifier,
                                                            ngap_ctrl_notifier,
                                                            task_sched,
                                                            rrc_ue_up_resource_manager,
                                                            logger);
}

async_task<cu_cp_ue_context_release_complete>
du_processor_routine_manager::start_ue_context_release_routine(const cu_cp_ue_context_release_command& command,
                                                               du_processor_cu_cp_notifier&            cu_cp_notifier)
{
  return launch_async<ue_context_release_routine>(
      command, e1ap_ctrl_notifier, f1ap_ue_ctxt_notifier, cu_cp_notifier, ue_manager, logger);
}

async_task<bool> du_processor_routine_manager::start_reestablishment_context_modification_routine(
    ue_index_t                                    ue_index,
    du_processor_rrc_ue_control_message_notifier& rrc_ue_ctrl_notifier,
    up_resource_manager&                          ue_up_resource_manager)
{
  return launch_async<reestablishment_context_modification_routine>(
      ue_index, e1ap_ctrl_notifier, f1ap_ue_ctxt_notifier, rrc_ue_ctrl_notifier, ue_up_resource_manager, logger);
}

async_task<cu_cp_inter_du_handover_response> du_processor_routine_manager::start_inter_du_handover_routine(
    const cu_cp_inter_du_handover_request&        command,
    du_processor_cu_cp_notifier&                  cu_cp_notifier,
    du_processor_f1ap_ue_context_notifier&        target_du_f1ap_ue_ctxt_notifier,
    du_processor_rrc_ue_control_message_notifier& rrc_ue_ctrl_notifier,
    up_resource_manager&                          ue_up_resource_manager)
{
  return launch_async<inter_du_handover_routine>(command,
                                                 cu_cp_notifier,
                                                 f1ap_ue_ctxt_notifier,
                                                 target_du_f1ap_ue_ctxt_notifier,
                                                 e1ap_ctrl_notifier,
                                                 ue_manager,
                                                 rrc_ue_ctrl_notifier,
                                                 ue_up_resource_manager,
                                                 logger);
}

async_task<cu_cp_inter_ngran_node_n2_handover_response>
du_processor_routine_manager::start_inter_ngran_node_n2_handover_routine(
    const cu_cp_inter_ngran_node_n2_handover_request& command,
    du_processor_ngap_control_notifier&               ngap_ctrl_notifier_)
{
  return launch_async<inter_ngran_node_n2_handover_routine>(command, ngap_ctrl_notifier_);
}

async_task<ngap_handover_resource_allocation_response>
du_processor_routine_manager::start_inter_cu_handover_target_routine(const ngap_handover_request& request_,
                                                                     du_processor_cu_cp_notifier& cu_cp_notifier)
{
  return launch_async<inter_cu_handover_target_routine>(request_,
                                                        f1ap_ue_ctxt_notifier,
                                                        e1ap_ctrl_notifier,
                                                        cu_cp_notifier,
                                                        ue_manager,
                                                        default_security_indication,
                                                        logger);
}
