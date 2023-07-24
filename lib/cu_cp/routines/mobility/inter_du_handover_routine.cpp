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
  source_ue = ue_manager.find_du_ue(command.source_ue_index);
  srsran_assert(source_ue != nullptr, "Can't find source UE index {}.", command.source_ue_index);
}

void inter_du_handover_routine::operator()(coro_context<async_task<cu_cp_inter_du_handover_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", command.source_ue_index, name());

  {
    // Create UE context for target DU w/o providing RNTI yet.
    target_ue = ue_manager.add_ue(command.target_du_index, command.neighbor_pci, {});
    if (target_ue == nullptr) {
      logger.error("ue={}: \"{}\" failed to generate UE context at target DU.", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
  }

  {
    // prepare F1AP UE Context Setup Command and call F1AP notifier of target DU
    if (!generate_ue_context_setup_request(
            target_ue_context_setup_request, source_ue->get_srbs(), ue_up_resource_manager.get_drbs())) {
      logger.error("ue={}: \"{}\" failed to generate UE context setup request at DU.", command.source_ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    CORO_AWAIT_VALUE(target_ue_context_setup_response,
                     target_du_f1ap_ue_ctxt_notifier.on_ue_context_setup_request(target_ue_context_setup_request));
  }

  // e1ap_ctrl_notifier.on_bearer_context_modification_request

  {
    ue_up_resource_manager.get_nof_drbs();
    // If there is an active E1AP context,
    // prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_modification_command.ue_index = command.source_ue_index;

    CORO_AWAIT(e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_command));
  }

  {
    // TODO: Trigger RRC Handover here
    ue_manager.find_du_ue(command.source_ue_index);
    // rrc_ue_notifier.on_rrc_reconfiguration_request()
    // rrc_du_notifier.on_ue_context_release_command(command.ue_index);
    CORO_AWAIT_VALUE(source_ue_context_modification_response,
                     source_du_f1ap_ue_ctxt_notifier.on_ue_context_modification_request(source_ue_context_mod_request));
  }

  CORO_RETURN(response_msg);
}

bool inter_du_handover_routine::generate_ue_context_setup_request(f1ap_ue_context_setup_request& setup_request,
                                                                  const std::map<srb_id_t, cu_srb_context>& srbs,
                                                                  const std::vector<drb_id_t>&              drbs)
{
  target_ue_context_setup_request.ue_index = target_ue->get_ue_index();

  // TODO: fill SRBs and DRBs to be setup

  return true;
}