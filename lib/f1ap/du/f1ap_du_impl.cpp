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

#include "f1ap_du_impl.h"
#include "../f1ap_asn1_utils.h"
#include "asn1_helpers.h"
#include "f1ap_du_connection_handler.h"
#include "log_helpers.h"
#include "procedures/f1ap_du_gnbdu_config_update_procedure.h"
#include "procedures/f1ap_du_initiated_reset_procedure.h"
#include "procedures/f1ap_du_positioning_procedures.h"
#include "procedures/f1ap_du_removal_procedure.h"
#include "procedures/f1ap_du_reset_procedure.h"
#include "procedures/f1ap_du_setup_procedure.h"
#include "procedures/f1ap_du_ue_context_release_procedure.h"
#include "procedures/f1ap_du_ue_context_setup_procedure.h"
#include "procedures/gnb_cu_configuration_update_procedure.h"
#include "ue_context/f1ap_du_ue_config_update.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ran/nr_cgi.h"

using namespace srsran;
using namespace asn1::f1ap;
using namespace srs_du;

class f1ap_du_impl::tx_pdu_notifier_with_logging final : public f1ap_message_notifier
{
public:
  tx_pdu_notifier_with_logging(f1ap_du_impl& parent_, std::unique_ptr<f1ap_message_notifier> decorated_) :
    parent(parent_), decorated(std::move(decorated_))
  {
  }

  void on_new_message(const f1ap_message& msg) override
  {
    // Log message.
    parent.log_pdu(false, msg);

    // Forward message to DU.
    decorated->on_new_message(msg);
  }

private:
  f1ap_du_impl&                          parent;
  std::unique_ptr<f1ap_message_notifier> decorated;
};

f1ap_du_impl::f1ap_du_impl(f1c_connection_client&      f1c_client_handler_,
                           f1ap_du_configurator&       du_mng_,
                           task_executor&              ctrl_exec_,
                           du_high_ue_executor_mapper& ue_exec_mapper_,
                           f1ap_du_paging_notifier&    paging_notifier_,
                           timer_manager&              timers_) :
  logger(srslog::fetch_basic_logger("DU-F1")),
  ctrl_exec(ctrl_exec_),
  du_mng(du_mng_),
  paging_notifier(paging_notifier_),
  connection_handler(f1c_client_handler_, *this, du_mng, ctrl_exec),
  ues(du_mng, ctrl_exec, ue_exec_mapper_, timers_),
  events(std::make_unique<f1ap_event_manager>(du_mng.get_timer_factory())),
  metrics(true)
{
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
f1ap_du_impl::~f1ap_du_impl() {}

bool f1ap_du_impl::connect_to_cu_cp()
{
  std::unique_ptr<f1ap_message_notifier> pdu_notifier = connection_handler.connect_to_cu_cp();
  if (pdu_notifier == nullptr) {
    return false;
  }

  if (logger.info.enabled()) {
    // Decorate notifier with logging, if the logger is enabled.
    tx_pdu_notifier.reset(new tx_pdu_notifier_with_logging(*this, std::move(pdu_notifier)));
  } else {
    tx_pdu_notifier = std::move(pdu_notifier);
  }

  // Update other components Tx PDU notifier.
  ues.update_tx_pdu_notifier(*tx_pdu_notifier);

  return true;
}

async_task<f1_setup_result> f1ap_du_impl::handle_f1_setup_request(const f1_setup_request_message& request)
{
  return launch_async<f1ap_du_setup_procedure>(request, *tx_pdu_notifier, *events, du_mng.get_timer_factory(), ctxt);
}

async_task<void> f1ap_du_impl::handle_f1_removal_request()
{
  return launch_async<f1ap_du_removal_procedure>(connection_handler, *tx_pdu_notifier, *events);
}

async_task<f1_reset_acknowledgement> f1ap_du_impl::handle_f1_reset_request(const f1_reset_request& req)
{
  return launch_async<f1ap_du_initiated_reset_procedure>(req, *tx_pdu_notifier, *events, ues);
}

async_task<gnbdu_config_update_response>
f1ap_du_impl::handle_du_config_update(const gnbdu_config_update_request& request)
{
  return launch_async<f1ap_du_gnbdu_config_update_procedure>(request, *tx_pdu_notifier, *events);
}

f1ap_ue_creation_response f1ap_du_impl::handle_ue_creation_request(const f1ap_ue_creation_request& msg)
{
  f1ap_ue_creation_response resp = create_f1ap_ue(msg, ues, ctxt, *events, metrics);
  if (resp.result) {
    logger.info("{}: F1 UE context created successfully.", ues[msg.ue_index].context);
  } else {
    logger.warning("ue={} crnti={}: F1 UE context failed to be created.", fmt::underlying(msg.ue_index), msg.c_rnti);
  }
  return resp;
}

f1ap_ue_configuration_response f1ap_du_impl::handle_ue_configuration_request(const f1ap_ue_configuration_request& msg)
{
  return update_f1ap_ue_config(msg, ues);
}

void f1ap_du_impl::handle_ue_deletion_request(du_ue_index_t ue_index)
{
  if (ues.contains(ue_index)) {
    logger.info("{}: F1 UE context removed.", ues[ue_index].context);
  }
  ues.remove_ue(ue_index);
  metrics.on_ue_removal(ue_index);
}

void f1ap_du_impl::handle_reset(const reset_s& msg)
{
  du_mng.schedule_async_task(launch_async<reset_procedure>(msg, du_mng, ues, *tx_pdu_notifier));
}

void f1ap_du_impl::handle_gnb_cu_configuration_update(const gnb_cu_cfg_upd_s& msg)
{
  du_mng.schedule_async_task(launch_async<gnb_cu_configuration_update_procedure>(msg, du_mng, *tx_pdu_notifier));
}

void f1ap_du_impl::handle_ue_context_setup_request(const asn1::f1ap::ue_context_setup_request_s& msg)
{
  du_ue_index_t du_ue_index = INVALID_DU_UE_INDEX;

  if (msg->gnb_du_ue_f1ap_id_present) {
    const gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);
    const f1ap_du_ue*         u                 = ues.find(gnb_du_ue_f1ap_id);
    if (u != nullptr) {
      du_ue_index = u->context.ue_index;
    }
  }
  if (du_ue_index == INVALID_DU_UE_INDEX) {
    // UE does not exist. Request free du ue index to schedule procedure.
    du_ue_index = du_mng.find_free_ue_index();
  }

  // Schedule UE Context Setup Procedure.
  du_mng.get_ue_handler(du_ue_index)
      .schedule_async_task(launch_async<f1ap_du_ue_context_setup_procedure>(msg, ues, du_mng, du_ue_index, ctxt));
}

void f1ap_du_impl::handle_ue_context_release_command(const asn1::f1ap::ue_context_release_cmd_s& msg)
{
  if (msg->old_gnb_du_ue_f1ap_id_present) {
    // If the old gNB-DU UE F1AP ID IE is included in the UE CONTEXT RELEASE COMMAND message, the gNB-DU shall
    // additionally release the UE context associated with the old gNB-DU UE F1AP ID.
    gnb_du_ue_f1ap_id_t old_gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->old_gnb_du_ue_f1ap_id);
    f1ap_du_ue*         old_ue                = ues.find(old_gnb_du_ue_f1ap_id);
    if (old_ue != nullptr) {
      du_mng.get_ue_handler(old_ue->context.ue_index)
          .schedule_async_task(du_mng.request_ue_removal(f1ap_ue_delete_request{old_ue->context.ue_index}));
    } else {
      logger.warning("old gNB-DU UE F1AP ID={} not found", fmt::underlying(old_gnb_du_ue_f1ap_id));
    }
  }

  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);
  f1ap_du_ue*         u                 = ues.find(gnb_du_ue_f1ap_id);
  if (u == nullptr) {
    logger.warning("Discarding UeContextReleaseCommand cause=Unrecognized gNB-DU UE F1AP ID={}",
                   fmt::underlying(gnb_du_ue_f1ap_id));
    // TODO: Handle.
    return;
  }

  du_mng.get_ue_handler(u->context.ue_index)
      .schedule_async_task(launch_async<f1ap_du_ue_context_release_procedure>(msg, ues));
}

void f1ap_du_impl::handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);
  f1ap_du_ue*         ue                = ues.find(gnb_du_ue_f1ap_id);

  if (ue == nullptr) {
    logger.error("Discarding UeContextModificationRequest cause=Unrecognized gNB-DU UE F1AP ID={}",
                 fmt::underlying(gnb_du_ue_f1ap_id));
    // TODO: Handle.
    return;
  }

  ue->handle_ue_context_modification_request(msg);
}

void f1ap_du_impl::handle_dl_rrc_message_transfer(const asn1::f1ap::dl_rrc_msg_transfer_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);

  // [TS38.473, 8.4.2.2.] If a UE-associated logical F1-connection exists, the DL RRC MESSAGE TRANSFER message shall
  // contain the gNB DU UE F1AP ID IE, which should be used by gNB-DU to lookup the stored UE context.
  f1ap_du_ue* ue = ues.find(gnb_du_ue_f1ap_id);

  if (ue == nullptr) {
    // [TS38.473, 8.4.2.2.] If no UE-associated logical F1-connection exists, the UE-associated logical F1-connection
    // shall be established at reception of the DL RRC MESSAGE TRANSFER message.
    logger.warning("du_ue={}: Discarding DLRRCMessageTransfer. Cause: No UE found with the provided gNB-DU-UE-F1AP-ID",
                   fmt::underlying(gnb_du_ue_f1ap_id));
    // TODO.
    return;
  }

  if (msg->old_gnb_du_ue_f1ap_id_present) {
    // [TS38.473, 8.4.2.2] The DL RRC MESSAGE TRANSFER message shall include, if available, the old gNB-DU UE F1AP ID
    // IE so that the gNB-DU can retrieve the existing UE context in RRC connection reestablishment procedure, as
    // defined in TS 38.401 [4].
    // [TS38.473, 8.4.2.2] If the gNB-DU identifies the UE-associated logical F1-connection by the gNB-DU UE F1AP ID IE
    // in the DL RRC MESSAGE TRANSFER message and the old gNB-DU UE F1AP ID IE is included, it shall release the old
    // gNB-DU UE F1AP ID and the related configurations associated with the old gNB-DU UE F1AP ID.
    const gnb_du_ue_f1ap_id_t old_gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->old_gnb_du_ue_f1ap_id);
    f1ap_du_ue*               old_ue                = ues.find(old_gnb_du_ue_f1ap_id);
    if (old_ue != nullptr) {
      // Delete gNB-CU-UE-F1AP-Id so that the UE is not reachable by the CU, and the gNB-CU-UE-F1AP-Id can be reused.
      old_ue->context.gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
      // Mark the old UE so that no UE context release procedure gets initiated.
      old_ue->context.marked_for_release = true;

      // Notify DU that the old UE needs to be released.
      du_mng.notify_reestablishment_of_old_ue(ue->context.ue_index, old_ue->context.ue_index);
    } else {
      logger.warning("old gNB-DU UE F1AP ID={} not found", fmt::underlying(old_gnb_du_ue_f1ap_id));
    }
  }

  // Handle gNB-CU UE F1AP ID.
  if (not handle_rx_message_gnb_cu_ue_f1ap_id(*ue, int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id))) {
    return;
  }

  if (msg->new_gnb_cu_ue_f1ap_id_present) {
    // If the DL RRC MESSAGE TRANSFER message contains the New gNB-CU UE F1AP ID IE, the gNB-DU shall, if supported,
    // replace the value received in the gNB-CU UE F1AP ID IE by the value of the New gNB-CU UE F1AP ID and use it for
    // further signalling.
    ue->context.gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(msg->new_gnb_cu_ue_f1ap_id);
  }

  if (ue->context.rrc_state == f1ap_ue_context::ue_rrc_state::no_config and not msg->old_gnb_du_ue_f1ap_id_present) {
    // If the UE has no dedicated configuration yet, we assume that this DL RRC Message Transfer contains it (e.g.
    // RRC Setup or RRC Reconfiguration). The only exception is when this DL RRC Message Transfer is an RRC
    // Reestablishment. In such case, we let the following DL RRC Message Transfer (with RRC Reconfiguration) to be the
    // one setting the state to config_pending.
    ue->context.rrc_state = f1ap_ue_context::ue_rrc_state::config_pending;
  }

  const srb_id_t srb_id     = int_to_srb_id(msg->srb_id);
  f1c_bearer*    srb_bearer = ue->bearers.find_srb(srb_id);
  if (srb_bearer == nullptr) {
    logger.warning("Discarding DLRRCMessageTransfer cause=SRB-ID={} not found", srb_id);
    cause_c cause;
    cause.set_radio_network().value = asn1::f1ap::cause_radio_network_opts::no_radio_res_available;
    send_error_indication(cause, std::nullopt, gnb_du_ue_f1ap_id, ue->context.gnb_cu_ue_f1ap_id);
    return;
  }

  // Forward SDU to lower layers.
  srb_bearer->handle_pdu(msg->rrc_container.copy(), msg->rrc_delivery_status_request_present);
}

void f1ap_du_impl::handle_ue_context_release_request(const f1ap_ue_context_release_request& request)
{
  f1ap_du_ue* ue = ues.find(request.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Discarding UeContextReleaseRequest. Cause: UE not found", fmt::underlying(request.ue_index));
    return;
  }

  if (ue->context.marked_for_release) {
    // UE context is already being released. Ignore the request.
    logger.debug(
        "ue={}: UE Context Release Request ignored. Cause: An UE Context Release procedure has already started.",
        fmt::underlying(request.ue_index));
    return;
  }

  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE_REQUEST);
  auto& rel_req = msg.pdu.init_msg().value.ue_context_release_request();

  rel_req->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ue->context.gnb_du_ue_f1ap_id);
  rel_req->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue->context.gnb_cu_ue_f1ap_id);

  // Set F1AP cause.
  using cause_type = f1ap_ue_context_release_request::cause_type;
  if (request.cause == cause_type::rlf_mac or request.cause == cause_type::rlf_rlc) {
    rel_req->cause.set_radio_network().value =
        (request.cause == cause_type::rlf_rlc ? cause_radio_network_opts::rl_fail_rlc
                                              : cause_radio_network_opts::rl_fail_others);
  } else {
    rel_req->cause.set_radio_network().value = cause_radio_network_opts::cell_not_available;
  }

  ue->f1ap_msg_notifier.on_new_message(msg);
}

async_task<f1ap_ue_context_modification_confirm>
f1ap_du_impl::handle_ue_context_modification_required(const f1ap_ue_context_modification_required& msg)
{
  // TODO: add procedure implementation

  f1ap_event_manager::f1ap_ue_context_modification_outcome_t ue_ctxt_mod_resp;

  return launch_async([this, ue_ctxt_mod_resp, res = f1ap_ue_context_modification_confirm{}](
                          coro_context<async_task<f1ap_ue_context_modification_confirm>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(ue_ctxt_mod_resp, events->f1ap_ue_context_modification_outcome);

    if (ue_ctxt_mod_resp.has_value()) {
      logger.debug("Received PDU with successful outcome");
      res.success = true;
    } else {
      logger.debug("Received PDU with unsuccessful outcome");
      res.success = false;
    }

    CORO_RETURN(res);
  });
}

void f1ap_du_impl::handle_message(const f1ap_message& msg)
{
  using pdu_types = f1ap_pdu_c::types_opts;

  // Run F1AP protocols in Control executor.
  if (not ctrl_exec.execute([this, msg]() {
        // Log message.
        log_pdu(true, msg);

        // Record metric.
        metrics.on_rx_pdu(msg);

        switch (msg.pdu.type().value) {
          case pdu_types::init_msg:
            handle_initiating_message(msg.pdu.init_msg());
            break;
          case pdu_types::successful_outcome:
            handle_successful_outcome(msg.pdu.successful_outcome());
            break;
          case pdu_types::unsuccessful_outcome:
            handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
            break;
          default:
            logger.error("Invalid PDU type");
            break;
        }
      })) {
    logger.error("Unable to dispatch handling of F1AP PDU. Cause: DU task queue is full");
    // TODO: Handle.
    return;
  }
}

void f1ap_du_impl::handle_initiating_message(const init_msg_s& msg)
{
  using msg_types = f1ap_elem_procs_o::init_msg_c::types_opts;

  switch (msg.value.type().value) {
    case msg_types::reset:
      handle_reset(msg.value.reset());
      break;
    case msg_types::gnb_cu_cfg_upd:
      handle_gnb_cu_configuration_update(msg.value.gnb_cu_cfg_upd());
      break;
    case msg_types::dl_rrc_msg_transfer:
      handle_dl_rrc_message_transfer(msg.value.dl_rrc_msg_transfer());
      break;
    case msg_types::ue_context_setup_request:
      handle_ue_context_setup_request(msg.value.ue_context_setup_request());
      break;
    case msg_types::ue_context_mod_request:
      handle_ue_context_modification_request(msg.value.ue_context_mod_request());
      break;
    case msg_types::ue_context_release_cmd:
      handle_ue_context_release_command(msg.value.ue_context_release_cmd());
      break;
    case msg_types::paging:
      handle_paging_request(msg.value.paging());
      break;
    case msg_types::positioning_meas_request:
      handle_positioning_measurement_request(msg.value.positioning_meas_request());
      break;
    case msg_types::trp_info_request:
      handle_trp_information_request(msg.value.trp_info_request());
      break;
    case msg_types::positioning_info_request:
      handle_positioning_information_request(msg.value.positioning_info_request());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void f1ap_du_impl::handle_successful_outcome(const asn1::f1ap::successful_outcome_s& outcome)
{
  std::optional<uint8_t> transaction_id = get_transaction_id(outcome);
  if (not transaction_id.has_value()) {
    logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
    return;
  }

  // Set transaction result and resume suspended procedure.
  if (not events->transactions.set_response(transaction_id.value(), outcome)) {
    logger.warning("Unexpected transaction id={}", transaction_id.value());
  }
}

void f1ap_du_impl::handle_unsuccessful_outcome(const asn1::f1ap::unsuccessful_outcome_s& outcome)
{
  std::optional<uint8_t> transaction_id = get_transaction_id(outcome);
  if (not transaction_id.has_value()) {
    logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
    return;
  }

  // Set transaction result and resume suspended procedure.
  if (not events->transactions.set_response(transaction_id.value(), make_unexpected(outcome))) {
    logger.warning("Unexpected transaction id={}", transaction_id.value());
  }
}

bool f1ap_du_impl::handle_rx_message_gnb_cu_ue_f1ap_id(f1ap_du_ue& ue, gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id)
{
  if (ue.context.gnb_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id) {
    // No update in gNB-CU UE F1AP ID.
    return true;
  }

  const f1ap_du_ue* ue_cu_id = ues.find(gnb_cu_ue_f1ap_id);
  if (ue.context.gnb_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid and ue_cu_id == nullptr) {
    // First message from the gNB-CU for this UE. Update gNB-CU-UE-F1AP-ID.
    ue.context.gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id;
    return true;
  }
  cause_c cause;
  if (ue_cu_id == nullptr) {
    cause.set_radio_network().value = cause_radio_network_opts::unknown_or_inconsistent_pair_of_ue_f1ap_id;
    logger.warning("Discarding message. Cause: gNB-CU UE F1AP ID={} does not match existing context",
                   fmt::underlying(gnb_cu_ue_f1ap_id));
  } else {
    // [TS38.473, Cause IE] The action failed because the gNB-CU UE F1AP ID is either unknown, or (for a first
    // message received at the gNB-CU) is known and already allocated to an existing context.
    cause.set_radio_network().value = cause_radio_network_opts::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
    logger.warning("Discarding message cause=gNB-CU UE F1AP ID={} is known and already allocated to an existing "
                   "context with gNB-DU UE F1AP ID={}",
                   fmt::underlying(gnb_cu_ue_f1ap_id),
                   fmt::underlying(ue_cu_id->context.gnb_du_ue_f1ap_id));
  }
  send_error_indication(cause, std::nullopt, ue.context.gnb_du_ue_f1ap_id, ue.context.gnb_cu_ue_f1ap_id);
  return false;
}

void f1ap_du_impl::send_error_indication(const asn1::f1ap::cause_c&         cause,
                                         std::optional<uint8_t>             transaction_id,
                                         std::optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                         std::optional<gnb_cu_ue_f1ap_id_t> cu_ue_id)
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_ERROR_IND);
  auto& err_ind = msg.pdu.init_msg().value.error_ind();

  if (transaction_id.has_value()) {
    err_ind->transaction_id = transaction_id.value();
  } else {
    auto transaction        = events->transactions.create_transaction();
    err_ind->transaction_id = transaction.id();
  }

  // Set cause.
  err_ind->cause_present = true;
  err_ind->cause         = cause;

  // [TS 38.473, 8.2.2.2] In case the Error Indication procedure is triggered by utilising UE associated signalling
  // the gNB-CU UE F1AP ID IE and gNBDU UE F1AP ID IE shall be included in the ERROR INDICATION message.
  err_ind->gnb_cu_ue_f1ap_id_present = cu_ue_id.has_value();
  if (err_ind->gnb_cu_ue_f1ap_id_present) {
    err_ind->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(*cu_ue_id);
  }
  err_ind->gnb_du_ue_f1ap_id_present = du_ue_id.has_value();
  if (err_ind->gnb_du_ue_f1ap_id_present) {
    err_ind->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(*du_ue_id);
  }

  // Send message to CU.
  tx_pdu_notifier->on_new_message(msg);
}

void f1ap_du_impl::handle_paging_request(const asn1::f1ap::paging_s& msg)
{
  paging_information info{};
  expected<unsigned> ue_identity_index_value = get_paging_ue_identity_index_value(msg);
  if (not ue_identity_index_value.has_value()) {
    logger.error("Discarding Paging message. Cause=Paging UE Identity index type {} is not supported",
                 msg->ue_id_idx_value.type().to_string());
    return;
  }
  info.ue_identity_index_value = ue_identity_index_value.value();

  expected<paging_identity_type> paging_type_indicator = get_paging_identity_type(msg);
  expected<uint64_t>             paging_identity       = get_paging_identity(msg);
  if (not paging_type_indicator.has_value()) {
    logger.error("Discarding Paging message. Cause=Paging Identity type {} is not supported",
                 msg->paging_id.type().to_string());
    return;
  }
  info.paging_type_indicator = paging_type_indicator.value();
  if (not paging_identity.has_value()) {
    logger.error("Discarding Paging message. Cause=Paging Identity type {} is not supported",
                 msg->paging_id.type().to_string());
    return;
  }
  info.paging_identity = paging_identity.value();

  if (msg->paging_drx_present) {
    expected<unsigned> paging_drx = get_paging_drx_in_nof_rf(msg);
    if (not paging_drx.has_value()) {
      logger.error("DRX value {} is not supported", msg->paging_drx.to_string());
    } else {
      info.paging_drx = paging_drx.value();
    }
  }
  if (msg->paging_prio_present) {
    expected<unsigned> paging_priority = get_paging_priority(msg);
    if (not paging_priority.has_value()) {
      logger.error("Paging priority value {} is not supported", msg->paging_prio.to_string());
    } else {
      info.paging_priority = paging_priority.value();
    }
  }
  if (msg->paging_origin_present and msg->paging_origin.value == paging_origin_e::non_neg3gpp) {
    info.is_paging_origin_non_3gpp_access = true;
  }
  for (const auto& asn_nr_cgi : msg->paging_cell_list) {
    const auto ret = cgi_from_asn1(asn_nr_cgi->paging_cell_item().nr_cgi);
    if (not ret.has_value()) {
      logger.error("Invalid CGI in paging cell list");
      continue;
    }
    auto                        paging_cell_cgi = ret.value();
    const f1ap_du_cell_context* cell_same_cgi   = ctxt.find_cell(paging_cell_cgi);
    if (cell_same_cgi == nullptr) {
      // Cell not served by this DU.
      logger.error("Cell with PLMN={} and NCI={} not handled by DU", paging_cell_cgi.plmn_id, paging_cell_cgi.nci);
      continue;
    }
    info.paging_cells.push_back(cell_same_cgi->cell_index);
  }
  paging_notifier.on_paging_received(info);
}

void f1ap_du_impl::handle_positioning_measurement_request(const positioning_meas_request_s& msg)
{
  du_mng.schedule_async_task(start_positioning_measurement_procedure(msg, du_mng, *tx_pdu_notifier));
}

void f1ap_du_impl::handle_trp_information_request(const trp_info_request_s& msg)
{
  du_mng.schedule_async_task(start_trp_information_exchange_procedure(msg, du_mng, *tx_pdu_notifier));
}

void f1ap_du_impl::handle_positioning_information_request(const asn1::f1ap::positioning_info_request_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);
  f1ap_du_ue*         ue                = ues.find(gnb_du_ue_f1ap_id);

  if (ue == nullptr) {
    // UE not found.
    // TODO: Handle
    return;
  }

  du_mng.get_ue_handler(ue->context.ue_index)
      .schedule_async_task(start_positioning_exchange_procedure(msg, du_mng, *ue));
}

gnb_cu_ue_f1ap_id_t f1ap_du_impl::get_gnb_cu_ue_f1ap_id(const du_ue_index_t& ue_index)
{
  gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
  const f1ap_du_ue*   ue                = ues.find(ue_index);
  if (ue) {
    gnb_cu_ue_f1ap_id = ue->context.gnb_cu_ue_f1ap_id;
  }
  return gnb_cu_ue_f1ap_id;
}

gnb_cu_ue_f1ap_id_t f1ap_du_impl::get_gnb_cu_ue_f1ap_id(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id)
{
  gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
  const f1ap_du_ue*   ue                = ues.find(gnb_du_ue_f1ap_id);
  if (ue) {
    gnb_cu_ue_f1ap_id = ue->context.gnb_cu_ue_f1ap_id;
  }
  return gnb_cu_ue_f1ap_id;
}

gnb_du_ue_f1ap_id_t f1ap_du_impl::get_gnb_du_ue_f1ap_id(const du_ue_index_t& ue_index)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_t::invalid;
  const f1ap_du_ue*   ue                = ues.find(ue_index);
  if (ue) {
    gnb_du_ue_f1ap_id = ue->context.gnb_du_ue_f1ap_id;
  }
  return gnb_du_ue_f1ap_id;
}
gnb_du_ue_f1ap_id_t f1ap_du_impl::get_gnb_du_ue_f1ap_id(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_t::invalid;
  const f1ap_du_ue*   ue                = ues.find(gnb_cu_ue_f1ap_id);
  if (ue) {
    gnb_du_ue_f1ap_id = ue->context.gnb_du_ue_f1ap_id;
  }
  return gnb_du_ue_f1ap_id;
}

du_ue_index_t f1ap_du_impl::get_ue_index(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id)
{
  du_ue_index_t     du_ue_index = du_ue_index_t::INVALID_DU_UE_INDEX;
  const f1ap_du_ue* ue          = ues.find(gnb_du_ue_f1ap_id);
  if (ue) {
    du_ue_index = ue->context.ue_index;
  }
  return du_ue_index;
}

du_ue_index_t f1ap_du_impl::get_ue_index(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id)
{
  du_ue_index_t     du_ue_index = du_ue_index_t::INVALID_DU_UE_INDEX;
  const f1ap_du_ue* ue          = ues.find(gnb_cu_ue_f1ap_id);
  if (ue) {
    du_ue_index = ue->context.ue_index;
  }
  return du_ue_index;
}

void f1ap_du_impl::log_pdu(bool is_rx, const f1ap_message& msg)
{
  using namespace asn1::f1ap;

  if (not logger.info.enabled()) {
    return;
  }

  // Fetch UE index.
  auto                         cu_ue_id = srsran::get_gnb_du_ue_f1ap_id(msg.pdu);
  std::optional<du_ue_index_t> ue_idx;
  if (cu_ue_id.has_value()) {
    auto* ue_ptr = ues.find(cu_ue_id.value());
    if (ue_ptr != nullptr and ue_ptr->context.ue_index != INVALID_DU_UE_INDEX) {
      ue_idx = ue_ptr->context.ue_index;
    }
  }

  // Log PDU.
  log_f1ap_pdu(logger, is_rx, ctxt.du_id, ue_idx, msg, logger.debug.enabled());
}
