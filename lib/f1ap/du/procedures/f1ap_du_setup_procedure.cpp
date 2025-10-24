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

#include "f1ap_du_setup_procedure.h"
#include "../../asn1_helpers.h"
#include "../../f1ap_asn1_utils.h"
#include "../f1ap_asn1_converters.h"
#include "../f1ap_du_context.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srsran::srs_du;
using namespace asn1::f1ap;

static constexpr std::chrono::milliseconds f1_setup_response_timeout{3000};

f1ap_du_setup_procedure::f1ap_du_setup_procedure(const f1_setup_request_message& request_,
                                                 f1ap_message_notifier&          cu_notif_,
                                                 f1ap_event_manager&             ev_mng_,
                                                 timer_factory&                  timers,
                                                 f1ap_du_context&                du_ctxt_) :
  request(request_),
  cu_notifier(cu_notif_),
  ev_mng(ev_mng_),
  logger(srslog::fetch_basic_logger("DU-F1")),
  du_ctxt(du_ctxt_),
  f1_setup_wait_timer(timers.create_timer())
{
}

void f1ap_du_setup_procedure::operator()(coro_context<async_task<f1_setup_result>>& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    transaction = ev_mng.transactions.create_transaction(f1_setup_response_timeout);
    if (not transaction.valid()) {
      CORO_EARLY_RETURN(create_f1_setup_result());
    }

    // Send request to CU.
    send_f1_setup_request();

    // Await CU response.
    CORO_AWAIT(transaction);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.debug("Received F1SetupFailure with Time to Wait IE - reinitiating F1 setup in {}s (retry={}/{})",
                 time_to_wait.count(),
                 f1_setup_retry_no,
                 request.max_setup_retries);
    CORO_AWAIT(
        async_wait_for(f1_setup_wait_timer, std::chrono::duration_cast<std::chrono::milliseconds>(time_to_wait)));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_f1_setup_result());
}

void f1ap_du_setup_procedure::send_f1_setup_request()
{
  // Save the gNB-DU-Id before the F1 Setup is completed for the purpose of logging.
  du_ctxt.du_id = request.gnb_du_id;

  f1ap_message msg = {};
  // Set F1AP PDU contents.
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  f1_setup_request_s& setup_req = msg.pdu.init_msg().value.f1_setup_request();

  setup_req->transaction_id = transaction.id();

  // DU-global parameters.
  setup_req->gnb_du_id           = static_cast<uint64_t>(request.gnb_du_id);
  setup_req->gnb_du_name_present = not request.gnb_du_name.empty();
  if (setup_req->gnb_du_name_present) {
    setup_req->gnb_du_name.from_string(request.gnb_du_name);
  }
  setup_req->gnb_du_rrc_version.latest_rrc_version.from_number(request.rrc_version);

  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.resize(request.served_cells.size());
  for (unsigned i = 0; i != request.served_cells.size(); ++i) {
    const f1_cell_setup_params& cell_cfg = request.served_cells[i];
    setup_req->gnb_du_served_cells_list[i].load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_LIST);
    gnb_du_served_cells_item_s& f1ap_cell = setup_req->gnb_du_served_cells_list[i]->gnb_du_served_cells_item();

    // Set base servedCellInfo
    f1ap_cell.served_cell_info = make_asn1_served_cell_info(cell_cfg.cell_info, cell_cfg.slices);

    // Add System Information related to the cell.
    f1ap_cell.gnb_du_sys_info_present  = true;
    f1ap_cell.gnb_du_sys_info.mib_msg  = cell_cfg.du_sys_info.packed_mib.copy();
    f1ap_cell.gnb_du_sys_info.sib1_msg = cell_cfg.du_sys_info.packed_sib1.copy();
  }

  // send request
  logger.info("F1 Setup: Sending F1 Setup Request to CU-CP...");
  cu_notifier.on_new_message(msg);
}

bool f1ap_du_setup_procedure::retry_required()
{
  if (transaction.aborted()) {
    // Timeout or cancellation case.
    return false;
  }

  const f1ap_transaction_response& cu_pdu_response = transaction.response();
  if (cu_pdu_response.has_value()) {
    // Success case.
    return false;
  }

  if (cu_pdu_response.error().value.type().value !=
      f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail) {
    // Invalid response type.
    return false;
  }

  const f1_setup_fail_ies_container& f1_setup_fail = *cu_pdu_response.error().value.f1_setup_fail();
  if (not f1_setup_fail.time_to_wait_present) {
    // CU didn't command a waiting time.
    logger.debug("CU-CP did not set any retry waiting time");
    return false;
  }
  if (f1_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of F1 Setup connection retries ({})", request.max_setup_retries);
    return false;
  }

  time_to_wait = std::chrono::seconds{f1_setup_fail.time_to_wait.to_number()};
  return true;
}

static bool validate_f1_response(const f1ap_transaction_response& transaction_resp)
{
  if (transaction_resp.has_value() and transaction_resp.value().value.type().value !=
                                           f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp) {
    // Unexpected successful outcome type.
    return false;
  }
  if (not transaction_resp.has_value() and transaction_resp.error().value.type().value !=
                                               f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail) {
    // Unexpected unsuccessful outcome type.
    return false;
  }

  return true;
}

f1_setup_result f1ap_du_setup_procedure::create_f1_setup_result()
{
  if (not transaction.valid()) {
    // Transaction could not be allocated.
    logger.error("{}: Procedure cancelled. Cause: Failed to allocate transaction.", name());
    f1_setup_failure fail{};
    fail.result   = f1_setup_failure::result_code::proc_failure;
    du_ctxt.du_id = gnb_du_id_t::invalid;
    return make_unexpected(fail);
  }

  if (transaction.aborted()) {
    // Abortion/timeout case.
    logger.error("{}: Procedure cancelled. Cause: Timeout reached.", name());
    f1_setup_failure fail{};
    fail.result   = f1_setup_failure::result_code::timeout;
    du_ctxt.du_id = gnb_du_id_t::invalid;
    return make_unexpected(fail);
  }
  const f1ap_transaction_response& cu_pdu_response = transaction.response();

  if (not validate_f1_response(cu_pdu_response)) {
    // Response content is invalid.
    logger.error(
        "{}: Received PDU with unexpected PDU type {}", name(), cu_pdu_response.value().value.type().to_string());
    f1_setup_failure fail{};
    fail.result   = f1_setup_failure::result_code::invalid_response;
    du_ctxt.du_id = gnb_du_id_t::invalid;
    return make_unexpected(fail);
  }

  if (not cu_pdu_response.has_value()) {
    // F1 Setup Failure case.
    const auto&      asn1_fail = *cu_pdu_response.error().value.f1_setup_fail();
    f1_setup_failure failure{};
    failure.result                 = f1_setup_failure::result_code::f1_setup_failure;
    failure.f1_setup_failure_cause = get_cause_str(asn1_fail.cause);
    logger.debug("{}: F1 Setup Failure with cause \"{}\"", name(), get_cause_str(asn1_fail.cause));
    du_ctxt.du_id = gnb_du_id_t::invalid;

    return make_unexpected(failure);
  }

  // F1 Setup Response case.

  const auto&      asn1_success = *cu_pdu_response.value().value.f1_setup_resp();
  f1_setup_success success{};

  // Update F1 DU Context (taking values from request).
  du_ctxt.du_id       = request.gnb_du_id;
  du_ctxt.gnb_du_name = request.gnb_du_name;
  du_ctxt.f1c_setup   = true;
  du_ctxt.served_cells.clear();
  for (unsigned i = 0, sz = request.served_cells.size(); i != sz; ++i) {
    const du_cell_index_t cell_idx = request.served_cells[i].cell_index;
    du_ctxt.served_cells.emplace(cell_idx,
                                 f1ap_du_cell_context{cell_idx,
                                                      request.served_cells[i].cell_info.nr_cgi,
                                                      request.served_cells[i].cell_info.ntn_link_rtt});
  }
  if (asn1_success.cells_to_be_activ_list_present) {
    success.cells_to_activate.resize(asn1_success.cells_to_be_activ_list.size());
    for (unsigned i = 0, sz = asn1_success.cells_to_be_activ_list.size(); i != sz; ++i) {
      auto&               asn1_cell    = asn1_success.cells_to_be_activ_list[i]->cells_to_be_activ_list_item();
      nr_cell_global_id_t cgi          = cgi_from_asn1(asn1_cell.nr_cgi).value();
      success.cells_to_activate[i].cgi = cgi;
    }
  }

  logger.info("{}: Procedure completed successfully.", name());

  return success;
}
