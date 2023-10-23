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

#include "ue_context_release_routine.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/ran/cause.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

ue_context_release_routine::ue_context_release_routine(const cu_cp_ue_context_release_command& command_,
                                                       du_processor_e1ap_control_notifier&     e1ap_ctrl_notif_,
                                                       du_processor_f1ap_ue_context_notifier&  f1ap_ue_ctxt_notif_,
                                                       du_processor_cu_cp_notifier&            cu_cp_notifier_,
                                                       du_processor_ue_manager&                ue_manager_,
                                                       srslog::basic_logger&                   logger_) :
  command(command_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  cu_cp_notifier(cu_cp_notifier_),
  ue_manager(ue_manager_),
  logger(logger_)
{
  srsran_assert(!command.cause.valueless_by_exception(), "Release command needs to be set.");
}

void ue_context_release_routine::operator()(coro_context<async_task<cu_cp_ue_context_release_complete>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", command.ue_index, name());

  // Prepare context release complete
  {
    release_complete.ue_index = command.ue_index;
    release_complete.pdu_session_res_list_cxt_rel_cpl =
        ue_manager.find_du_ue(command.ue_index)->get_up_resource_manager().get_pdu_sessions();

    // Call RRC UE notifier to get the release context of the UE and add the location info to the UE context release
    // complete message
    rrc_ue_release_context release_context =
        ue_manager.find_du_ue(command.ue_index)->get_rrc_ue_notifier().get_rrc_ue_release_context();
    release_complete.user_location_info = release_context.user_location_info;
  }

  if (not ue_manager.find_du_ue(command.ue_index)->get_up_resource_manager().get_pdu_sessions().empty()) {
    // If there is an active E1AP context,
    // prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_release_command.ue_index = command.ue_index;
    bearer_context_release_command.cause    = cause_radio_network_t::unspecified;

    CORO_AWAIT(e1ap_ctrl_notifier.on_bearer_context_release_command(bearer_context_release_command));
  }

  {
    // prepare F1AP UE Context Release Command and call F1AP notifier
    f1ap_ue_context_release_cmd.ue_index        = command.ue_index;
    f1ap_ue_context_release_cmd.cause           = cause_radio_network_t::unspecified;
    f1ap_ue_context_release_cmd.rrc_release_pdu = command.rrc_release_pdu.copy();
    f1ap_ue_context_release_cmd.srb_id          = command.srb_id;

    CORO_AWAIT_VALUE(f1ap_ue_context_release_result,
                     f1ap_ue_ctxt_notifier.on_ue_context_release_command(f1ap_ue_context_release_cmd));
  }

  {
    // Remove UE
    cu_cp_notifier.on_ue_removal_required(command.ue_index);
  }

  CORO_RETURN(release_complete);
}
