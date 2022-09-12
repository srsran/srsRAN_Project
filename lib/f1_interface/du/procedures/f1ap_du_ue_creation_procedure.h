/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../f1ap_du_ue.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {
namespace srs_du {

/// \brief Creates an UE object in the F1AP and SRB0.
/// \return Result of the F1 UE creation that contains the outcome (success/failure) and list of created F1c bearers.
f1ap_ue_create_response create_f1ap_du_ue(const f1ap_ue_create_request& msg,
                                          f1ap_du_ue_manager&           ues,
                                          const f1ap_du_context&        du_ctx,
                                          f1c_message_notifier&         f1c_notifier,
                                          f1ap_event_manager&           ev_mng)
{
  const asn1::f1ap::gnb_du_served_cells_item_s& ue_pcell = du_ctx.served_cells[msg.cell_index];

  f1ap_du_ue& u  = ues.add_ue(msg.ue_index);
  u.context.rnti = msg.c_rnti;

  // Create an F1c bearer for each requested SRB.
  for (const srb_to_addmod& srb : msg.srbs_to_add) {
    if (srb.srb_id == srb_id_t::srb0) {
      auto srb0 = std::make_unique<f1c_srb0_du_bearer>(
          u.context, ue_pcell.served_cell_info.nrcgi, msg.du_cu_rrc_container, f1c_notifier, ev_mng);
      u.bearers.push_back(std::move(srb0));
    } else {
      auto srb1 = std::make_unique<f1c_other_srb_du_bearer>(u.context, srb.srb_id, f1c_notifier);
      u.bearers.push_back(std::move(srb1));
    }
  }

  // Prepare response.
  f1ap_ue_create_response resp{};
  resp.result = true;
  for (std::unique_ptr<f1_bearer>& bearer : u.bearers) {
    resp.bearers_added.push_back(bearer.get());
  }
  return resp;
}

} // namespace srs_du
} // namespace srsgnb
