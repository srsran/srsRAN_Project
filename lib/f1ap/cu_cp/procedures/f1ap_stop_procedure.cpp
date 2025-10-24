/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "f1ap_stop_procedure.h"
#include "../ue_context/f1ap_cu_ue_context.h"

using namespace srsran;
using namespace srs_cu_cp;

f1ap_stop_procedure::f1ap_stop_procedure(f1ap_du_processor_notifier& cu_cp_notifier_,
                                         f1ap_ue_context_list&       ue_ctxt_list_) :
  cu_cp_notifier(cu_cp_notifier_), ue_ctxt_list(ue_ctxt_list_)
{
}

void f1ap_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Stop all UEs associated with this DU.
  if (ue_ctxt_list.size() > 0) {
    CORO_AWAIT(handle_transaction_info_loss());
  }

  // Stop all the common transactions.
  // TODO

  CORO_RETURN();
}

async_task<void> f1ap_stop_procedure::handle_transaction_info_loss()
{
  ue_transaction_info_loss_event ev;
  ev.ues_lost.reserve(ue_ctxt_list.size());

  // Add DU UEs to the list of UEs with transaction information lost.
  ev.ues_lost.reserve(ue_ctxt_list.size());
  for (auto& ue : ue_ctxt_list) {
    ev.ues_lost.push_back(ue.second.ue_ids.ue_index);
  }

  // After receiving an F1 Removal Request, no more F1AP Rx PDUs are expected. Cancel running UE F1AP transactions.
  // Note: size of ue_list may change during this operation (e.g. if a concurrent UE context release was being
  // processed and got cancelled). Therefore, we leverage the list ev.ues_lost for the iteration.
  for (ue_index_t ue_idx : ev.ues_lost) {
    auto* u = ue_ctxt_list.find(ue_idx);
    if (u != nullptr) {
      u->ev_mng.cancel_all();
    }
  }
  ev.ues_lost.erase(std::remove_if(ev.ues_lost.begin(),
                                   ev.ues_lost.end(),
                                   [this](ue_index_t ue_idx) { return ue_ctxt_list.find(ue_idx) == nullptr; }),
                    ev.ues_lost.end());

  return cu_cp_notifier.on_transaction_info_loss(std::move(ev));
}
