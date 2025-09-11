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

#include "e1ap_cu_up_release_procedure.h"
#include "common/e1ap_common_messages.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_ies.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"

using namespace srsran;
using namespace srs_cu_up;
using namespace asn1::e1ap;

e1ap_cu_up_release_procedure::e1ap_cu_up_release_procedure(e1ap_cu_up_connection_handler& cu_up_conn_handler_,
                                                           e1ap_message_notifier&         tx_pdu_notifier_,
                                                           e1ap_event_manager&            ev_mng_,
                                                           srslog::basic_logger&          logger_) :
  cu_up_conn_handler(cu_up_conn_handler_), cu_notifier(tx_pdu_notifier_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1ap_cu_up_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  // Check if the TNL is still up.
  if (not cu_up_conn_handler.is_connected()) {
    CORO_EARLY_RETURN();
  }

  // Send the E1AP release request and await response.
  transaction = ev_mng.transactions.create_transaction(std::chrono::milliseconds{1000});

  // Send request to CU-CP.
  send_e1ap_release_request();

  // Await CU-CP response.
  CORO_AWAIT(transaction);

  handle_e1ap_release_response();

  // TODO: Tear down TNL association and await it.

  CORO_RETURN();
}

void e1ap_cu_up_release_procedure::send_e1ap_release_request()
{
  e1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_E1AP_ID_E1_RELEASE);
  asn1::e1ap::e1_release_request_s& release_request = msg.pdu.init_msg().value.e1_release_request();

  release_request->transaction_id            = transaction.id();
  release_request->cause.set_radio_network() = asn1::e1ap::cause_radio_network_opts::normal_release;

  // Send unpacked message to E1-C GW.
  cu_notifier.on_new_message(msg);
}

void e1ap_cu_up_release_procedure::handle_e1ap_release_response()
{
  if (transaction.aborted()) {
    // Timeout or cancellation case.
    logger.warning(
        "{}: Forcing shutdown of E1 TNL association. Cause: Timeout reached for reception of the E1 Release Response",
        name());
  } else {
    const asn1::e1ap::successful_outcome_s& success = transaction.response().value();
    if (success.value.type().value != e1ap_elem_procs_o::successful_outcome_c::types_opts::e1_release_resp) {
      logger.warning("{}: Received unexpected E1AP PDU type \"{}\"", name(), success.value.type().to_string());

      cause_c cause;
      cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
      cu_notifier.on_new_message(generate_error_indication(transaction.id(), {}, {}, cause));
      return;
    }
    if (success.value.e1_release_resp()->transaction_id != transaction.id()) {
      logger.warning("{}: Received E1 Release Response with wrong transaction ID", name());
      return;
    }
    logger.debug("\"{}\" finished successfully", name());
  }
}
