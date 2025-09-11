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

#include "f1ap_du_initiated_reset_procedure.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "f1ap_du_event_manager.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/f1ap_message_notifier.h"

using namespace srsran;
using namespace srs_du;

f1ap_du_initiated_reset_procedure::f1ap_du_initiated_reset_procedure(const f1_reset_request& req_,
                                                                     f1ap_message_notifier&  msg_notifier_,
                                                                     f1ap_event_manager&     ev_mng_,
                                                                     f1ap_du_ue_manager&     ues_) :
  req(req_), msg_notifier(msg_notifier_), ev_mng(ev_mng_), ues(ues_), logger(srslog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_initiated_reset_procedure::operator()(coro_context<async_task<f1_reset_acknowledgement>>& ctx)
{
  CORO_BEGIN(ctx);

  transaction = ev_mng.transactions.create_transaction();
  if (not transaction.valid()) {
    CORO_EARLY_RETURN(handle_f1_reset_completion());
  }

  // Send F1 RESET to CU-CP.
  send_f1_reset();

  // Await CU response.
  CORO_AWAIT(transaction);

  CORO_RETURN(handle_f1_reset_completion());
}

void f1ap_du_initiated_reset_procedure::send_f1_reset()
{
  using namespace asn1::f1ap;

  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_RESET);
  reset_s& ie = msg.pdu.init_msg().value.reset();

  ie->transaction_id = transaction.id();

  switch (req.cause) {
    case f1_reset_request::cause_type::cell_removal:
      ie->cause.set_radio_network().value = cause_radio_network_opts::cell_not_available;
      break;
    case f1_reset_request::cause_type::other:
    default:
      ie->cause.set_misc().value = cause_misc_opts::unspecified;
      break;
  }

  if (not req.ues_reset.empty()) {
    // Fill UEs to be reset.
    ue_associated_lc_f1_conn_list_res_l& lst = ie->reset_type.set_part_of_f1_interface();
    lst.reserve(req.ues_reset.size());
    for (du_ue_index_t ue_idx : req.ues_reset) {
      auto* u = ues.find(ue_idx);
      if (u != nullptr) {
        asn1::protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_o> prot_ie;
        prot_ie.load_info_obj(ASN1_F1AP_ID_UE_ASSOCIATED_LC_F1_CONN_ITEM);
        ue_associated_lc_f1_conn_item_s& item = prot_ie->ue_associated_lc_f1_conn_item();
        item.gnb_du_ue_f1ap_id_present        = true;
        item.gnb_du_ue_f1ap_id                = gnb_du_ue_f1ap_id_to_uint(u->context.gnb_du_ue_f1ap_id);
        if (u->context.gnb_cu_ue_f1ap_id != gnb_cu_ue_f1ap_id_t::invalid) {
          item.gnb_cu_ue_f1ap_id_present = true;
          item.gnb_cu_ue_f1ap_id         = gnb_cu_ue_f1ap_id_to_uint(u->context.gnb_cu_ue_f1ap_id);
        }
        lst.push_back(std::move(prot_ie));
      } else {
        logger.warning("ue={}: UE not found. It will be skipped in the F1 Reset message.", fmt::underlying(ue_idx));
      }
    }
  } else {
    ie->reset_type.set_f1_interface().value = reset_all_opts::reset_all;
  }

  // Send message to CU-CP.
  msg_notifier.on_new_message(msg);
}

f1_reset_acknowledgement f1ap_du_initiated_reset_procedure::handle_f1_reset_completion() const
{
  using namespace asn1::f1ap;

  if (not transaction.valid()) {
    // Transaction could not be allocated.
    logger.error("{}: Procedure cancelled. Cause: Could not allocate new transaction.", name());
    return f1_reset_acknowledgement{false};
  }

  if (transaction.aborted()) {
    // Abortion/timeout case.
    logger.error("{}: Procedure cancelled. Cause: Timeout reached.", name());
    return f1_reset_acknowledgement{false};
  }
  const f1ap_transaction_response& cu_pdu_response = transaction.response();

  if (not cu_pdu_response.has_value() or cu_pdu_response->proc_code != ASN1_F1AP_ID_RESET) {
    // Response content is invalid.
    logger.error(
        "{}: Received PDU with unexpected PDU type {}", name(), cu_pdu_response.value().value.type().to_string());
    return f1_reset_acknowledgement{false};
  }

  // Note: For now, we do not handle the content of the RESET ACK

  return f1_reset_acknowledgement{true};
}
