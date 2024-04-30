/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_routine_manager.h"
#include "../routines/pdu_session_resource_modification_routine.h"
#include "../routines/pdu_session_resource_release_routine.h"
#include "../routines/pdu_session_resource_setup_routine.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_routine_manager::cu_cp_routine_manager(ue_manager&                  ue_mng_,
                                             const security_indication_t& default_security_indication_,
                                             srslog::basic_logger&        logger_) :

  ue_mng(ue_mng_),
  default_security_indication(default_security_indication_),
  logger(logger_),

  main_ctrl_loop(128)
{
}

bool cu_cp_routine_manager::schedule_async_task(async_task<void> task)
{
  return main_ctrl_loop.schedule(std::move(task));
}

async_task<cu_cp_pdu_session_resource_setup_response> cu_cp_routine_manager::start_pdu_session_resource_setup_routine(
    const cu_cp_pdu_session_resource_setup_request& setup_msg,
    const srsran::security::sec_as_config&          security_cfg,
    e1ap_bearer_context_manager&                    e1ap_bearer_ctxt_mng,
    f1ap_ue_context_manager&                        f1ap_ue_ctxt_mng,
    du_processor_rrc_ue_control_message_notifier&   rrc_ue_ctrl_notifier,
    up_resource_manager&                            rrc_ue_up_resource_manager)
{
  return launch_async<pdu_session_resource_setup_routine>(setup_msg,
                                                          ue_mng.get_ue_config(),
                                                          security_cfg,
                                                          default_security_indication,
                                                          e1ap_bearer_ctxt_mng,
                                                          f1ap_ue_ctxt_mng,
                                                          rrc_ue_ctrl_notifier,
                                                          rrc_ue_up_resource_manager,
                                                          logger);
}

async_task<cu_cp_pdu_session_resource_modify_response>
cu_cp_routine_manager::start_pdu_session_resource_modification_routine(
    const cu_cp_pdu_session_resource_modify_request& modify_msg,
    e1ap_bearer_context_manager&                     e1ap_bearer_ctxt_mng,
    f1ap_ue_context_manager&                         f1ap_ue_ctxt_mng,
    du_processor_rrc_ue_control_message_notifier&    rrc_ue_ctrl_notifier,
    up_resource_manager&                             rrc_ue_up_resource_manager)
{
  return launch_async<pdu_session_resource_modification_routine>(
      modify_msg, e1ap_bearer_ctxt_mng, f1ap_ue_ctxt_mng, rrc_ue_ctrl_notifier, rrc_ue_up_resource_manager, logger);
}

async_task<cu_cp_pdu_session_resource_release_response>
cu_cp_routine_manager::start_pdu_session_resource_release_routine(
    const cu_cp_pdu_session_resource_release_command& release_cmd,
    e1ap_bearer_context_manager&                      e1ap_bearer_ctxt_mng,
    f1ap_ue_context_manager&                          f1ap_ue_ctxt_mng,
    du_processor_ngap_control_notifier&               ngap_ctrl_notifier,
    du_processor_rrc_ue_control_message_notifier&     rrc_ue_ctrl_notifier,
    du_processor_ue_task_scheduler&                   task_sched,
    up_resource_manager&                              rrc_ue_up_resource_manager)
{
  return launch_async<pdu_session_resource_release_routine>(release_cmd,
                                                            e1ap_bearer_ctxt_mng,
                                                            f1ap_ue_ctxt_mng,
                                                            ngap_ctrl_notifier,
                                                            rrc_ue_ctrl_notifier,
                                                            task_sched,
                                                            rrc_ue_up_resource_manager,
                                                            logger);
}
