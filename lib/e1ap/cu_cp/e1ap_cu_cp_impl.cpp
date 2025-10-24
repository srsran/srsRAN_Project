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

#include "e1ap_cu_cp_impl.h"
#include "../common/e1ap_asn1_helpers.h"
#include "../common/log_helpers.h"
#include "cu_cp/procedures/cu_cp_e1_reset_procedure.h"
#include "cu_cp/procedures/e1_release_procedure.h"
#include "cu_cp/procedures/e1ap_stop_procedure.h"
#include "e1ap_cu_cp_asn1_helpers.h"
#include "procedures/bearer_context_modification_procedure.h"
#include "procedures/bearer_context_release_procedure.h"
#include "procedures/bearer_context_setup_procedure.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/cause/e1ap_cause.h"
#include "srsran/ran/cause/e1ap_cause_converters.h"
#include <variant>

using namespace srsran;
using namespace asn1::e1ap;
using namespace srs_cu_cp;

// ------ e1ap_cu_cp_impl ------

e1ap_cu_cp_impl::e1ap_cu_cp_impl(const e1ap_configuration&      e1ap_cfg_,
                                 cu_up_index_t                  cu_up_index_,
                                 e1ap_message_notifier&         e1ap_pdu_notifier_,
                                 e1ap_cu_up_processor_notifier& e1ap_cu_up_processor_notifier_,
                                 e1ap_cu_cp_notifier&           cu_cp_notifier_,
                                 timer_manager&                 timers_,
                                 task_executor&                 ctrl_exec_,
                                 unsigned                       max_nof_supported_ues_) :
  e1ap_cfg(e1ap_cfg_),
  cu_up_index(cu_up_index_),
  logger(srslog::fetch_basic_logger("CU-CP-E1")),
  pdu_notifier(e1ap_message_notifier_with_logging(*this, e1ap_pdu_notifier_)),
  cu_up_processor_notifier(e1ap_cu_up_processor_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  ctrl_exec(ctrl_exec_),
  timers(timer_factory{timers_, ctrl_exec_}),
  ue_ctxt_list(timers, max_nof_supported_ues_, logger),
  ev_mng(timers)
{
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
e1ap_cu_cp_impl::~e1ap_cu_cp_impl() {}

async_task<void> e1ap_cu_cp_impl::stop()
{
  e1ap_stopping = true;
  return launch_async<e1ap_stop_procedure>(cu_cp_notifier, ue_ctxt_list);
}

async_task<void> e1ap_cu_cp_impl::handle_cu_cp_e1_reset_message(const cu_cp_reset& reset)
{
  if (e1ap_stopping) {
    logger.debug("Dropping E1 Reset message as E1AP is stopping");
    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  if (!std::holds_alternative<e1ap_cause_t>(reset.cause)) {
    logger.error("Invalid cause type for E1 Reset");
    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  return launch_async<cu_cp_e1_reset_procedure>(e1ap_cfg, reset, pdu_notifier, ev_mng, ue_ctxt_list, logger);
}

void e1ap_cu_cp_impl::handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& msg)
{
  // Pack message into PDU.
  e1ap_message e1ap_msg;
  if (msg.success) {
    e1ap_msg.pdu.set_successful_outcome();
    e1ap_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);
    auto& setup_resp = e1ap_msg.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp();

    if (msg.gnb_cu_cp_name.has_value()) {
      setup_resp->gnb_cu_cp_name.from_string(msg.gnb_cu_cp_name.value());
    }

    // Set values handled by E1AP.
    e1ap_msg.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp()->transaction_id = current_transaction_id;

    // Send response.
    pdu_notifier.on_new_message(e1ap_msg);
  } else {
    e1ap_msg.pdu.set_unsuccessful_outcome();
    e1ap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);
    e1ap_msg.pdu.unsuccessful_outcome().value.gnb_cu_up_e1_setup_fail();
    auto& setup_fail  = e1ap_msg.pdu.unsuccessful_outcome().value.gnb_cu_up_e1_setup_fail();
    setup_fail->cause = cause_to_asn1(msg.cause.value());

    // Set values handled by E1.
    setup_fail->transaction_id = current_transaction_id;
    setup_fail->cause.set_radio_network();
    setup_fail->cause.radio_network() = asn1::e1ap::cause_radio_network_opts::options::no_radio_res_available;

    // Send response.
    pdu_notifier.on_new_message(e1ap_msg);
  }
}

async_task<e1ap_bearer_context_setup_response>
e1ap_cu_cp_impl::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& request)
{
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = ue_ctxt_list.allocate_gnb_cu_cp_ue_e1ap_id();
  if (cu_cp_ue_e1ap_id == gnb_cu_cp_ue_e1ap_id_t::invalid) {
    logger.warning("No CU-CP-UE-E1AP-ID available");
    return launch_async([](coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);
      e1ap_bearer_context_setup_response res;
      res.success = false;
      CORO_RETURN(res);
    });
  }

  // Add new e1ap_ue_context.
  if (ue_ctxt_list.add_ue(request.ue_index, cu_cp_ue_e1ap_id) == nullptr) {
    logger.warning("Bearer Context Setup failed. Cause: bearer context already exists");
    return launch_async([](coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);
      e1ap_bearer_context_setup_response res;
      res.success = false;
      CORO_RETURN(res);
    });
  }

  e1ap_ue_context& ue_ctxt = ue_ctxt_list[cu_cp_ue_e1ap_id];

  e1ap_message e1ap_msg;
  e1ap_msg.pdu.set_init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  auto& bearer_context_setup_request = e1ap_msg.pdu.init_msg().value.bearer_context_setup_request();

  fill_asn1_bearer_context_setup_request(bearer_context_setup_request, request);
  bearer_context_setup_request->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(ue_ctxt.ue_ids.cu_cp_ue_e1ap_id);

  return launch_async<bearer_context_setup_procedure>(
      e1ap_cfg, e1ap_msg, ue_ctxt.bearer_ev_mng, ue_ctxt_list, pdu_notifier, ue_ctxt.logger);
}

async_task<e1ap_bearer_context_modification_response>
e1ap_cu_cp_impl::handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& request)
{
  if (!ue_ctxt_list.contains(request.ue_index)) {
    logger.warning("ue={}: Dropping BearerContextModificationRequest. Bearer context does not exist", request.ue_index);
    return launch_async([](coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);
      e1ap_bearer_context_modification_response res{};
      res.success = false;
      res.cause   = cause_misc_t::unspecified;
      CORO_RETURN(res);
    });
  }

  // Get UE context.
  e1ap_ue_context& ue_ctxt = ue_ctxt_list[request.ue_index];

  e1ap_message e1ap_msg;
  e1ap_msg.pdu.set_init_msg();
  e1ap_msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);

  auto& bearer_context_mod_request                 = e1ap_msg.pdu.init_msg().value.bearer_context_mod_request();
  bearer_context_mod_request->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(ue_ctxt.ue_ids.cu_cp_ue_e1ap_id);
  bearer_context_mod_request->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(ue_ctxt.ue_ids.cu_up_ue_e1ap_id);

  fill_asn1_bearer_context_modification_request(bearer_context_mod_request, request);

  return launch_async<bearer_context_modification_procedure>(
      e1ap_cfg, e1ap_msg, ue_ctxt.bearer_ev_mng, pdu_notifier, ue_ctxt.logger);
}

async_task<void>
e1ap_cu_cp_impl::handle_bearer_context_release_command(const e1ap_bearer_context_release_command& command)
{
  if (!ue_ctxt_list.contains(command.ue_index)) {
    logger.info("ue={}: Dropping BearerContextReleaseCommand. Cause: Bearer context does not exist", command.ue_index);
    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  if (e1_release_in_progress) {
    logger.debug("ue={}: Dropping BearerContextReleaseCommand. Cause: E1 Release in progress", command.ue_index);
    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  // Get UE context.
  e1ap_ue_context& ue_ctxt = ue_ctxt_list[command.ue_index];

  e1ap_message e1ap_msg;
  e1ap_msg.pdu.set_init_msg();
  e1ap_msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  auto& bearer_context_release_cmd                 = e1ap_msg.pdu.init_msg().value.bearer_context_release_cmd();
  bearer_context_release_cmd->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(ue_ctxt.ue_ids.cu_cp_ue_e1ap_id);
  bearer_context_release_cmd->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(ue_ctxt.ue_ids.cu_up_ue_e1ap_id);

  fill_asn1_bearer_context_release_command(bearer_context_release_cmd, command);

  return launch_async<bearer_context_release_procedure>(
      e1ap_cfg, e1ap_msg, command.ue_index, ue_ctxt_list, pdu_notifier);
}

void e1ap_cu_cp_impl::handle_message(const e1ap_message& msg)
{
  // Run E1AP protocols in Control executor.
  if (not ctrl_exec.execute([this, msg]() {
        // Log PDU.
        log_pdu(true, msg);

        switch (msg.pdu.type().value) {
          case asn1::e1ap::e1ap_pdu_c::types_opts::init_msg:
            handle_initiating_message(msg.pdu.init_msg());
            break;
          case asn1::e1ap::e1ap_pdu_c::types_opts::successful_outcome:
            handle_successful_outcome(msg.pdu.successful_outcome());
            break;
          case asn1::e1ap::e1ap_pdu_c::types_opts::unsuccessful_outcome:
            handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
            break;
          default:
            logger.error("Invalid PDU type");
            break;
        }
      })) {
    logger.warning("Discarding E1AP PDU. Cause: CU-CP task queue is full");
  }
}

void e1ap_cu_cp_impl::handle_initiating_message(const asn1::e1ap::init_msg_s& msg)
{
  using init_types = asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts;
  switch (msg.value.type().value) {
    case init_types::gnb_cu_up_e1_setup_request: {
      current_transaction_id     = msg.value.gnb_cu_up_e1_setup_request()->transaction_id;
      cu_up_e1_setup_request req = {};
      fill_e1ap_cu_up_e1_setup_request(req, msg.value.gnb_cu_up_e1_setup_request());
      cu_up_processor_notifier.on_cu_up_e1_setup_request_received(req);
    } break;
    case init_types::bearer_context_release_request: {
      handle_bearer_context_release_request(msg.value.bearer_context_release_request());
    } break;
    case init_types::bearer_context_inactivity_notif: {
      handle_bearer_context_inactivity_notification(msg.value.bearer_context_inactivity_notif());
    } break;
    case init_types::e1_release_request: {
      // Set E1 Release in progress flag.
      e1_release_in_progress = true;
      cu_up_processor_notifier.schedule_async_task(launch_async<e1_release_procedure>(
          msg.value.e1_release_request(), cu_up_index, pdu_notifier, cu_cp_notifier, ue_ctxt_list, timers, logger));
    } break;
    default:
      logger.warning("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void e1ap_cu_cp_impl::handle_bearer_context_release_request(const asn1::e1ap::bearer_context_release_request_s& msg)
{
  if (!ue_ctxt_list.contains(int_to_gnb_cu_cp_ue_e1ap_id(msg->gnb_cu_cp_ue_e1ap_id))) {
    logger.warning(
        "cu_cp_ue_e1ap_id={} cu_up_ue_e1ap_id={}: Dropping BearerContextReleaseRequest. UE context does not exist",
        msg->gnb_cu_cp_ue_e1ap_id,
        msg->gnb_cu_up_ue_e1ap_id);
    return;
  }

  // Get UE context.
  e1ap_ue_context& ue_ctxt = ue_ctxt_list[int_to_gnb_cu_cp_ue_e1ap_id(msg->gnb_cu_cp_ue_e1ap_id)];

  cu_cp_bearer_context_release_request release_request;
  release_request.ue_index = ue_ctxt.ue_ids.ue_index;
  release_request.cause    = e1ap_to_ngap_cause(asn1_to_cause(msg->cause));

  // Schedule forwarding of release request.
  if (!cu_cp_notifier.schedule_async_task(ue_ctxt.ue_ids.ue_index,
                                          launch_async([this, release_request](coro_context<async_task<void>>& ctx) {
                                            CORO_BEGIN(ctx);
                                            cu_cp_notifier.on_bearer_context_release_request_received(release_request);
                                            CORO_RETURN();
                                          }))) {
    logger.warning("ue={}: Dropping BearerContextReleaseRequest. UE does not exist", ue_ctxt.ue_ids.ue_index);
  }
}

void e1ap_cu_cp_impl::handle_bearer_context_inactivity_notification(
    const asn1::e1ap::bearer_context_inactivity_notif_s& msg)
{
  if (!ue_ctxt_list.contains(int_to_gnb_cu_cp_ue_e1ap_id(msg->gnb_cu_cp_ue_e1ap_id))) {
    logger.warning(
        "cu_cp_ue_e1ap_id={} cu_up_ue_e1ap_id={}: Dropping InactivityNotification. UE context does not exist",
        msg->gnb_cu_cp_ue_e1ap_id,
        msg->gnb_cu_up_ue_e1ap_id);
    return;
  }

  // Get UE context.
  e1ap_ue_context& ue_ctxt = ue_ctxt_list[int_to_gnb_cu_cp_ue_e1ap_id(msg->gnb_cu_cp_ue_e1ap_id)];

  cu_cp_inactivity_notification inactivity_notification;
  inactivity_notification.ue_index = ue_ctxt.ue_ids.ue_index;

  switch (msg->activity_info.type()) {
    // DRB activity notification level.
    case asn1::e1ap::activity_info_c::types_opts::options::drb_activity_list: {
      // Add inactive DRBs to common type message.
      for (const auto& drb : msg->activity_info.drb_activity_list()) {
        if (drb.drb_activity == asn1::e1ap::drb_activity_opts::options::not_active) {
          inactivity_notification.inactive_drbs.emplace_back(uint_to_drb_id(drb.drb_id));
        }
      }
      // If no DRBs are inactive, return.
      if (inactivity_notification.inactive_drbs.empty()) {
        return;
      }

      // If all DRBs are inactive, release UE.
      if (inactivity_notification.inactive_drbs.size() == msg->activity_info.drb_activity_list().size()) {
        inactivity_notification.ue_inactive = true;
      }
    } break;
    // PDU Session activity notification level.
    case asn1::e1ap::activity_info_c::types_opts::options::pdu_session_res_activity_list: {
      // add inactive pdu sessions to common type message
      for (const auto& pdu_session : msg->activity_info.pdu_session_res_activity_list()) {
        if (pdu_session.pdu_session_res_activity == asn1::e1ap::pdu_session_res_activity_opts::options::not_active) {
          inactivity_notification.inactive_pdu_sessions.emplace_back(
              uint_to_pdu_session_id(pdu_session.pdu_session_id));
        }
      }
      // If no PDU sessions are inactive, return.
      if (inactivity_notification.inactive_pdu_sessions.empty()) {
        return;
      }

      // If all PDU sessions are inactive, release UE.
      if (inactivity_notification.inactive_pdu_sessions.size() ==
          msg->activity_info.pdu_session_res_activity_list().size()) {
        inactivity_notification.ue_inactive = true;
      }
    } break;
    // UE activity notification level.
    case asn1::e1ap::activity_info_c::types_opts::options::ue_activity: {
      if (msg->activity_info.ue_activity() == asn1::e1ap::ue_activity_opts::options::not_active) {
        // If UE is inactive, release UE.
        inactivity_notification.ue_inactive = true;
      }
    } break;
    default:
      ue_ctxt.logger.log_error("Received Bearer Context Inactivity Notification with unsupported type {}",
                               msg->activity_info.type().to_string());
      return;
  }

  // Schedule forwarding of notification.
  if (!cu_cp_notifier.schedule_async_task(
          ue_ctxt.ue_ids.ue_index, launch_async([this, inactivity_notification](coro_context<async_task<void>>& ctx) {
            CORO_BEGIN(ctx);
            cu_cp_notifier.on_bearer_context_inactivity_notification_received(inactivity_notification);
            CORO_RETURN();
          }))) {
    logger.warning("ue={}: Dropping InactivityNotification. UE does not exist", ue_ctxt.ue_ids.ue_index);
  }
}

void e1ap_cu_cp_impl::handle_successful_outcome(const asn1::e1ap::successful_outcome_s& outcome)
{
  using successful_types                         = asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts;
  std::optional<gnb_cu_cp_ue_e1ap_id_t> cu_ue_id = get_gnb_cu_cp_ue_e1ap_id(outcome);

  if (cu_ue_id.has_value()) {
    if (not ue_ctxt_list.contains(*cu_ue_id)) {
      logger.warning("cu_ue={}: Discarding received \"{}\". Cause: UE was not found.",
                     fmt::underlying(*cu_ue_id),
                     outcome.value.type().to_string());
      return;
    }
  }

  switch (outcome.value.type().value) {
    case successful_types::bearer_context_setup_resp: {
      ue_ctxt_list[*cu_ue_id].bearer_ev_mng.context_setup_outcome.set(outcome.value.bearer_context_setup_resp());
    } break;
    case successful_types::bearer_context_mod_resp: {
      ue_ctxt_list[*cu_ue_id].bearer_ev_mng.context_modification_outcome.set(outcome.value.bearer_context_mod_resp());
    } break;
    case successful_types::bearer_context_release_complete: {
      ue_ctxt_list[*cu_ue_id].bearer_ev_mng.context_release_complete.set(
          outcome.value.bearer_context_release_complete());
    } break;
    default:
      // Handle successful outcomes with transaction ID.
      std::optional<uint8_t> transaction_id = get_transaction_id(outcome);
      if (not transaction_id.has_value()) {
        logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }

      // Set transaction result and resume suspended procedure.
      if (not ev_mng.transactions.set_response(transaction_id.value(), outcome)) {
        logger.warning("Ignoring message. Cause: Transaction with id={} has already completed", transaction_id.value());
      }
  }
}

void e1ap_cu_cp_impl::handle_unsuccessful_outcome(const asn1::e1ap::unsuccessful_outcome_s& outcome)
{
  using unsuccessful_types                       = asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts;
  std::optional<gnb_cu_cp_ue_e1ap_id_t> cu_ue_id = get_gnb_cu_cp_ue_e1ap_id(outcome);

  if (cu_ue_id.has_value()) {
    if (not ue_ctxt_list.contains(*cu_ue_id)) {
      logger.warning("cu_ue={}: Discarding received \"{}\". Cause: UE was not found.",
                     fmt::underlying(*cu_ue_id),
                     outcome.value.type().to_string());
      return;
    }
  }

  switch (outcome.value.type().value) {
    case unsuccessful_types::bearer_context_setup_fail: {
      ue_ctxt_list[*cu_ue_id].bearer_ev_mng.context_setup_outcome.set(outcome.value.bearer_context_setup_fail());
    } break;
    case unsuccessful_types::bearer_context_mod_fail: {
      ue_ctxt_list[*cu_ue_id].bearer_ev_mng.context_modification_outcome.set(outcome.value.bearer_context_mod_fail());
    } break;
    default:
      // Handle unsuccessful outcomes with transaction ID.
      std::optional<uint8_t> transaction_id = get_transaction_id(outcome);
      if (not transaction_id.has_value()) {
        logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }

      // Set transaction result and resume suspended procedure.
      if (not ev_mng.transactions.set_response(transaction_id.value(), make_unexpected(outcome))) {
        logger.warning("Ignoring message. Cause: Transaction with id={} has already completed", transaction_id.value());
      }
  }
}

void e1ap_cu_cp_impl::cancel_ue_tasks(ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.info("ue={}: UE does not exist in the CU-CP", ue_index);
    return;
  }

  auto& u = ue_ctxt_list[ue_index];
  u.bearer_ev_mng.cancel_all();
}

void e1ap_cu_cp_impl::update_ue_context(ue_index_t ue_index, ue_index_t old_ue_index)
{
  if (!ue_ctxt_list.contains(old_ue_index)) {
    logger.warning("Failed to transfer E1AP UE context from ue={} to ue={}. Old UE context does not exist",
                   old_ue_index,
                   ue_index);
    return;
  }

  ue_ctxt_list[old_ue_index].logger.log_debug("Transferring E1AP UE context to ue={}", ue_index);

  ue_ctxt_list.update_ue_index(ue_index, old_ue_index);
}

void e1ap_cu_cp_impl::remove_bearer_context(ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.debug("ue={}: UE context not found", ue_index);
    return;
  }

  ue_ctxt_list.remove_ue(ue_index);
}

void e1ap_cu_cp_impl::log_pdu(bool is_rx, const e1ap_message& e1ap_pdu)
{
  if (not logger.info.enabled()) {
    return;
  }

  // Fetch UE index.
  auto                      cp_ue_id = get_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu);
  std::optional<ue_index_t> ue_idx;
  if (cp_ue_id.has_value()) {
    auto* ue_ptr = ue_ctxt_list.find_ue(cp_ue_id.value());
    if (ue_ptr != nullptr and ue_ptr->ue_ids.ue_index != ue_index_t::invalid) {
      ue_idx = ue_ptr->ue_ids.ue_index;
    }
  }

  log_e1ap_pdu(logger, is_rx, ue_idx, e1ap_pdu, e1ap_cfg.json_log_enabled);
}

// ---- e1ap_message_notifier_with_logging

e1ap_cu_cp_impl::e1ap_message_notifier_with_logging::e1ap_message_notifier_with_logging(
    e1ap_cu_cp_impl&       parent_,
    e1ap_message_notifier& notifier_) :
  parent(parent_), notifier(notifier_)
{
}

void e1ap_cu_cp_impl::e1ap_message_notifier_with_logging::on_new_message(const e1ap_message& msg)
{
  // Log message.
  parent.log_pdu(false, msg);

  // Forward message to notifier.
  notifier.on_new_message(msg);
}
