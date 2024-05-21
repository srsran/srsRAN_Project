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

#include "f1_removal_procedure.h"
#include "../ue_context/f1ap_cu_ue_context.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

using namespace srsran;
using namespace srs_cu_cp;

f1_removal_procedure::f1_removal_procedure(const asn1::f1ap::f1_removal_request_s& request_,
                                           f1ap_message_notifier&                  pdu_notifier_,
                                           f1ap_du_processor_notifier&             cu_cp_notifier_,
                                           f1ap_ue_context_list&                   ue_list_,
                                           srslog::basic_logger&                   logger_) :
  request(request_), pdu_notifier(pdu_notifier_), cu_cp_notifier(cu_cp_notifier_), ue_list(ue_list_), logger(logger_)
{
}

void f1_removal_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // If there are still active UEs, reset their context in other layers.
  if (ue_list.size() > 0) {
    CORO_AWAIT(handle_ue_transaction_info_loss());
  }

  // Send F1 Removal Response
  send_f1_removal_response();

  CORO_RETURN();
}

async_task<void> f1_removal_procedure::handle_ue_transaction_info_loss()
{
  f1_ue_transaction_info_loss_event ev;

  // Add DU UEs to the list of UEs with transaction information lost.
  ev.ues_lost.reserve(ue_list.size());
  for (auto& ue : ue_list) {
    ev.ues_lost.push_back(ue.second.ue_ids.ue_index);
  }

  // After receiving an F1 Removal Request, no more F1AP Rx PDUs are expected. Cancel running UE F1AP transactions.
  // Note: size of ue_list may change during this operation (e.g. if a concurrent UE context release was being
  // processed and got cancelled). Therefore, we leverage the list ev.ues_lost for the iteration.
  for (ue_index_t ue_idx : ev.ues_lost) {
    auto* u = ue_list.find(ue_idx);
    if (u != nullptr) {
      u->ev_mng.cancel_all();
    }
  }
  ev.ues_lost.erase(std::remove_if(ev.ues_lost.begin(),
                                   ev.ues_lost.end(),
                                   [this](ue_index_t ue_idx) { return ue_list.find(ue_idx) == nullptr; }),
                    ev.ues_lost.end());

  logger.info("{}: F1 Removal Request received, but there are still UEs connected to the DU. Resetting {} UEs.",
              name(),
              ev.ues_lost.size());

  return cu_cp_notifier.on_transaction_info_loss(std::move(ev));
}

void f1_removal_procedure::send_f1_removal_response()
{
  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  asn1::f1ap::f1_removal_resp_s& rem = resp.pdu.successful_outcome().value.f1_removal_resp();
  rem->transaction_id                = (*request)[0]->transaction_id();
  pdu_notifier.on_new_message(resp);
}