/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_processor_routine_manager.h"
#include "../routines/mobility/inter_cu_handover_target_routine.h"
#include "../routines/mobility/inter_du_handover_routine.h"
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
    const cu_cp_inter_du_handover_request& command,
    du_processor_cu_cp_notifier&           cu_cp_notifier,
    du_processor_f1ap_ue_context_notifier& source_du_f1ap_ue_ctxt_notifier,
    du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notifier,
    du_processor_ue_context_notifier&      source_du_processor_notifier,
    du_processor_ue_context_notifier&      target_du_processor_notifier)
{
  return launch_async<inter_du_handover_routine>(command,
                                                 cu_cp_notifier,
                                                 source_du_f1ap_ue_ctxt_notifier,
                                                 target_du_f1ap_ue_ctxt_notifier,
                                                 e1ap_ctrl_notifier,
                                                 source_du_processor_notifier,
                                                 target_du_processor_notifier,
                                                 ue_manager,
                                                 logger);
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
