/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  f1_ue_transaction_info_loss_event ev;
  ev.ues_lost.reserve(ue_ctxt_list.size());

  for (auto& ue : ue_ctxt_list) {
    // No more F1AP Rx PDUs are expected. Cancel running UE F1AP transactions.
    ue.second.ev_mng.cancel_all();

    // Add UE to the list of UEs with transaction information lost.
    ev.ues_lost.push_back(ue.second.ue_ids.ue_index);
  }

  return cu_cp_notifier.on_transaction_info_loss(std::move(ev));
}
