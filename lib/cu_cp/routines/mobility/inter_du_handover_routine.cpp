/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "inter_du_handover_routine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

inter_du_handover_routine::inter_du_handover_routine(
    const cu_cp_inter_du_handover_request& command_,
    du_processor_f1ap_ue_context_notifier& source_du_f1ap_ue_ctxt_notif_,
    du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_,
    du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
    du_processor_ue_manager&               ue_manager_,
    up_resource_manager&                   ue_up_resource_manager_,
    srslog::basic_logger&                  logger_) :
  command(command_),
  source_du_f1ap_ue_ctxt_notifier(source_du_f1ap_ue_ctxt_notif_),
  target_du_f1ap_ue_ctxt_notifier(target_du_f1ap_ue_ctxt_notif_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  ue_manager(ue_manager_),
  ue_up_resource_manager(ue_up_resource_manager_),
  logger(logger_)
{
}

void inter_du_handover_routine::operator()(coro_context<async_task<cu_cp_inter_du_handover_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", command.ue_index, name());

  {
    // prepare F1AP UE Context Setup Command and call F1AP notifier
    target_ue_context_setup_request.ue_index = command.ue_index;
    // f1ap_ue_context_release_cmd.cause           = command.cause;
    // f1ap_ue_context_release_cmd.rrc_release_pdu = command.rrc_release_pdu.copy();
    // f1ap_ue_context_release_cmd.srb_id          = command.srb_id;

    CORO_AWAIT_VALUE(target_ue_context_setup_response,
                     target_du_f1ap_ue_ctxt_notifier.on_ue_context_setup_request(target_ue_context_setup_request));
  }

  // e1ap_ctrl_notifier.on_bearer_context_modification_request

  {
    ue_up_resource_manager.get_nof_drbs();
    // If there is an active E1AP context,
    // prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_modification_command.ue_index = command.ue_index;

    CORO_AWAIT(e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_command));
  }

  {
    // TODO: Trigger RRC Handover here
    ue_manager.find_du_ue(command.ue_index);
    // rrc_ue_notifier.on_rrc_reconfiguration_request()
    // rrc_du_notifier.on_ue_context_release_command(command.ue_index);
    CORO_AWAIT_VALUE(source_ue_context_modification_response,
                     source_du_f1ap_ue_ctxt_notifier.on_ue_context_modification_request(source_ue_context_mod_request));
  }

  CORO_RETURN(response_msg);
}
