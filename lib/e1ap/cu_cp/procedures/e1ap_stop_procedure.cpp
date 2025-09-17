/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_stop_procedure.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"

using namespace srsran;
using namespace srs_cu_cp;

e1ap_stop_procedure::e1ap_stop_procedure(e1ap_cu_cp_notifier& cu_cp_notifier_, e1ap_ue_context_list& ue_ctxt_list_) :
  cu_cp_notifier(cu_cp_notifier_), ue_ctxt_list(ue_ctxt_list_)
{
}

void e1ap_stop_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Stop all UEs associated with this CU-UP.
  if (ue_ctxt_list.size() > 0) {
    CORO_AWAIT(handle_transaction_info_loss());
  }

  // Stop all the common transactions.
  // TODO

  CORO_RETURN();
}

async_task<void> e1ap_stop_procedure::handle_transaction_info_loss()
{
  ue_transaction_info_loss_event ev;
  ev.ues_lost.reserve(ue_ctxt_list.size());

  // Add CU-UP UEs to the list of UEs with transaction information lost.
  ev.ues_lost.reserve(ue_ctxt_list.size());
  for (auto& ue : ue_ctxt_list) {
    ev.ues_lost.push_back(ue.second.ue_ids.ue_index);
  }

  // After receiving an E1 Release Request, no more E1AP Rx PDUs are expected. Cancel running UE E1AP transactions.
  // Note: size of ue_list may change during this operation (e.g. if a concurrent UE context release was being
  // processed and got cancelled). Therefore, we leverage the list ev.ues_lost for the iteration.
  for (ue_index_t ue_idx : ev.ues_lost) {
    auto* u = ue_ctxt_list.find_ue(ue_idx);
    if (u != nullptr) {
      u->bearer_ev_mng.cancel_all();
    }
  }
  ev.ues_lost.erase(std::remove_if(ev.ues_lost.begin(),
                                   ev.ues_lost.end(),
                                   [this](ue_index_t ue_idx) { return ue_ctxt_list.find_ue(ue_idx) == nullptr; }),
                    ev.ues_lost.end());

  return cu_cp_notifier.on_transaction_info_loss(std::move(ev));
}
