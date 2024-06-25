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

#include "ue_context_release_routine.h"
#include "srsran/ran/cause/ngap_cause_converters.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

ue_context_release_routine::ue_context_release_routine(const cu_cp_ue_context_release_command& command_,
                                                       e1ap_bearer_context_manager*            e1ap_bearer_ctxt_mng_,
                                                       f1ap_ue_context_manager&                f1ap_ue_ctxt_mng_,
                                                       cu_cp_ue_removal_handler&               ue_removal_handler_,
                                                       ue_manager&                             ue_mng_,
                                                       srslog::basic_logger&                   logger_) :
  command(command_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  f1ap_ue_ctxt_mng(f1ap_ue_ctxt_mng_),
  ue_removal_handler(ue_removal_handler_),
  ue_mng(ue_mng_),
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
        ue_mng.find_du_ue(command.ue_index)->get_up_resource_manager().get_pdu_sessions();

    // Call RRC UE notifier to get the release context of the UE and add the location info to the UE context release
    // complete message
    release_context = ue_mng.find_du_ue(command.ue_index)
                          ->get_rrc_ue_notifier()
                          .get_rrc_ue_release_context(command.requires_rrc_release);
    release_complete.user_location_info = release_context.user_location_info;
  }

  if (e1ap_bearer_ctxt_mng != nullptr and
      not ue_mng.find_du_ue(command.ue_index)->get_up_resource_manager().get_pdu_sessions().empty()) {
    // If there is an active E1AP context,
    // prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_release_command.ue_index = command.ue_index;
    bearer_context_release_command.cause    = ngap_to_e1ap_cause(command.cause);

    CORO_AWAIT(e1ap_bearer_ctxt_mng->handle_bearer_context_release_command(bearer_context_release_command));
  }

  {
    // prepare F1AP UE Context Release Command and call F1AP notifier
    f1ap_ue_context_release_cmd.ue_index = command.ue_index;
    f1ap_ue_context_release_cmd.cause    = ngap_to_f1ap_cause(command.cause);
    if (command.requires_rrc_release) {
      f1ap_ue_context_release_cmd.rrc_release_pdu = release_context.rrc_release_pdu.copy();
      f1ap_ue_context_release_cmd.srb_id          = release_context.srb_id;
    }

    CORO_AWAIT_VALUE(f1ap_ue_context_release_result,
                     f1ap_ue_ctxt_mng.handle_ue_context_release_command(f1ap_ue_context_release_cmd));
  }

  {
    // Remove UE
    CORO_AWAIT(ue_removal_handler.handle_ue_removal_request(command.ue_index));
    // Note: From this point the UE is removed and only the stored context can be accessed.
  }

  CORO_RETURN(release_complete);
}
