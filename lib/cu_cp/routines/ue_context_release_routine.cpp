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

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

ue_context_release_routine::ue_context_release_routine(const rrc_ue_context_release_command&  command_,
                                                       du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
                                                       du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notif_,
                                                       du_processor_rrc_du_ue_notifier&       rrc_du_notifier_,
                                                       du_processor_ue_manager&               ue_manager_,
                                                       up_resource_manager&                   ue_up_resource_manager_,
                                                       srslog::basic_logger&                  logger_) :
  command(command_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  rrc_du_notifier(rrc_du_notifier_),
  ue_manager(ue_manager_),
  ue_up_resource_manager(ue_up_resource_manager_),
  logger(logger_)
{
  srsran_assert(command.cause != cause_t::nulltype, "Release command needs to be set.");
}

void ue_context_release_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", command.ue_index, name());

  if (not ue_up_resource_manager.get_pdu_sessions().empty()) {
    // If there is an active E1AP context,
    // prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_release_command.ue_index = command.ue_index;
    bearer_context_release_command.cause    = command.cause;

    CORO_AWAIT(e1ap_ctrl_notifier.on_bearer_context_release_command(bearer_context_release_command));
  }

  {
    // prepare F1AP UE Context Release Command and call F1AP notifier
    f1ap_ue_context_release_cmd.ue_index        = command.ue_index;
    f1ap_ue_context_release_cmd.cause           = command.cause;
    f1ap_ue_context_release_cmd.rrc_release_pdu = command.rrc_release_pdu.copy();
    f1ap_ue_context_release_cmd.srb_id          = command.srb_id;

    CORO_AWAIT_VALUE(f1ap_ue_context_release_result,
                     f1ap_ue_ctxt_notifier.on_ue_context_release_command(f1ap_ue_context_release_cmd));
  }

  {
    // Remove UE from RRC
    rrc_du_notifier.on_ue_context_release_command(command.ue_index);

    // Remove UE from UE database
    logger.info("Removing UE (id={})", command.ue_index);
    ue_manager.remove_du_ue(command.ue_index);
  }

  CORO_RETURN();
}
