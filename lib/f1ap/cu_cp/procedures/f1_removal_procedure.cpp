/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                                           du_index_t                              du_index_,
                                           f1ap_message_notifier&                  pdu_notifier_,
                                           f1ap_du_processor_notifier&             cu_cp_notifier_,
                                           f1ap_ue_context_list&                   ue_list_,
                                           srslog::basic_logger&                   logger_) :
  request(request_),
  du_index(du_index_),
  pdu_notifier(pdu_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  ue_list(ue_list_),
  logger(logger_)
{
}

static std::vector<ue_index_t> get_list_of_ues_to_reset(const f1ap_ue_context_list& ue_list)
{
  std::vector<ue_index_t> vec;
  vec.reserve(ue_list.size());
  for (const auto& ue : ue_list) {
    vec.push_back(ue.second.ue_ids.ue_index);
  }
  return vec;
}

void f1_removal_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // If there are still active UEs, reset their context in other layers.
  if (ue_list.size() > 0) {
    CORO_AWAIT(cu_cp_notifier.on_ue_reset_required(get_list_of_ues_to_reset(ue_list)));
  }

  // Send F1 Removal Response
  send_f1_removal_response();

  CORO_RETURN();
}

void f1_removal_procedure::send_f1_removal_response()
{
  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  asn1::f1ap::f1_removal_resp_s& rem = resp.pdu.successful_outcome().value.f1_removal_resp();
  rem->transaction_id                = (*request)[0]->transaction_id();
  pdu_notifier.on_new_message(resp);
}