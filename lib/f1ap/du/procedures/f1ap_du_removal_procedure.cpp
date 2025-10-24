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

#include "f1ap_du_removal_procedure.h"
#include "../../f1ap_common_messages.h"
#include "../f1ap_du_context.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_removal_procedure::f1ap_du_removal_procedure(f1ap_du_connection_handler& du_conn_handler_,
                                                     f1ap_message_notifier&      tx_pdu_notifier_,
                                                     f1ap_event_manager&         ev_mng_,
                                                     f1ap_du_context&            du_ctxt_) :
  du_conn_handler(du_conn_handler_),
  cu_notifier(tx_pdu_notifier_),
  ev_mng(ev_mng_),
  du_ctxt(du_ctxt_),
  logger(srslog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_removal_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Check if the TNL is still up.
  if (not du_conn_handler.is_connected()) {
    CORO_EARLY_RETURN();
  }

  // Send the F1AP DU removal request and await response.
  transaction = ev_mng.transactions.create_transaction(std::chrono::milliseconds{1000});
  if (not transaction.valid()) {
    // Just shutdown the TNL association and finish procedure.
    logger.error("{}: Unable to allocate transaction. Shutting down F1 TNL association...", name());
    CORO_AWAIT(du_conn_handler.handle_tnl_association_removal());
    logger.info("{}: F1 TNL association shut down", name());
    CORO_EARLY_RETURN();
  }

  // Set the F1 state as not setup, regardless of the outcome.
  du_ctxt.f1c_setup = false;

  // Send request to CU.
  send_f1_removal_request();

  // Await CU response.
  CORO_AWAIT(transaction);

  handle_f1_removal_response();

  // Await for the TNL association tear down.
  logger.debug("{}: Shutting down F1 TNL association...", name());
  CORO_AWAIT(du_conn_handler.handle_tnl_association_removal());
  logger.info("{}: F1 TNL association shut down", name());

  CORO_RETURN();
}

void f1ap_du_removal_procedure::send_f1_removal_request()
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  asn1::f1ap::f1_removal_request_s& rem = msg.pdu.init_msg().value.f1_removal_request();

  rem->resize(1);
  (*rem)[0].load_info_obj(ASN1_F1AP_ID_TRANSACTION_ID);
  (*rem)[0].value().transaction_id() = transaction.id();

  // Send unpacked message to F1-C GW.
  cu_notifier.on_new_message(msg);
}

void f1ap_du_removal_procedure::handle_f1_removal_response()
{
  if (transaction.aborted()) {
    // Timeout or cancellation case.
    logger.warning(
        "{}: Forcing shutdown of F1 TNL association. Cause: Timeout reached for reception of the F1 Setup Response.",
        name());
  } else if (transaction.response().has_value()) {
    const asn1::f1ap::successful_outcome_s& success = transaction.response().value();
    if (success.value.type().value != f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_removal_resp) {
      logger.warning("{}: Received unexpected F1AP PDU type \"{}\"", name(), success.value.type().to_string());

      cause_c cause;
      cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
      cu_notifier.on_new_message(generate_error_indication(transaction.id(), {}, {}, cause));
      return;
    }
    if (success.value.f1_removal_resp()->transaction_id != transaction.id()) {
      logger.warning("{}: Received F1 Removal Response with wrong transaction ID.", name());
      return;
    }
    logger.debug("{}: F1 session successfully removed", name());
  } else {
    const asn1::f1ap::unsuccessful_outcome_s& unsuccess = transaction.response().error();
    if (unsuccess.value.type().value != f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_removal_fail) {
      logger.warning("{}: Received unexpected F1AP PDU type \"{}\"", name(), unsuccess.value.type().to_string());

      cause_c cause;
      cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
      cu_notifier.on_new_message(generate_error_indication(transaction.id(), {}, {}, cause));
      return;
    }
    // What else can we do?
    logger.warning("{}: Forcing shutdown of F1 TNL association after F1 Removal Failure", name());
  }
}
