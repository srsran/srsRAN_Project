/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_impl.h"
#include "../../ran/gnb_format.h"
#include "procedures/f1ap_du_setup_procedure.h"
#include "procedures/f1ap_du_ue_release_procedure.h"
#include "procedures/gnb_cu_configuration_update_procedure.h"
#include "ue_context/f1ap_du_ue_config_update.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/support/async/event_signal.h"

using namespace srsran;
using namespace asn1::f1ap;
using namespace srs_du;

f1ap_du_impl::f1ap_du_impl(f1ap_message_notifier&      message_notifier_,
                           f1ap_du_configurator&       du_mng_,
                           task_executor&              ctrl_exec_,
                           du_high_ue_executor_mapper& ue_exec_mapper_) :
  logger(srslog::fetch_basic_logger("DU-F1")),
  f1ap_notifier(message_notifier_),
  ctrl_exec(ctrl_exec_),
  ue_exec_mapper(ue_exec_mapper_),
  du_mng(du_mng_),
  ues(du_mng_, f1ap_notifier),
  events(std::make_unique<f1ap_event_manager>(du_mng.get_timer_manager()))
{
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
f1ap_du_impl::~f1ap_du_impl() {}

async_task<f1_setup_response_message> f1ap_du_impl::handle_f1_setup_request(const f1_setup_request_message& request)
{
  return launch_async<f1ap_du_setup_procedure>(request, f1ap_notifier, *events, du_mng.get_timer_manager(), ctxt);
}

f1ap_ue_creation_response f1ap_du_impl::handle_ue_creation_request(const f1ap_ue_creation_request& msg)
{
  return create_f1ap_ue(msg, ues, ctxt, *events);
}

f1ap_ue_configuration_response f1ap_du_impl::handle_ue_configuration_request(const f1ap_ue_configuration_request& msg)
{
  return update_f1ap_ue_config(msg, ues);
}

void f1ap_du_impl::handle_gnb_cu_configuration_update(const asn1::f1ap::gnb_cu_cfg_upd_s& msg)
{
  du_mng.schedule_async_task(launch_async<gnb_cu_configuration_update_procedure>(msg, f1ap_notifier));
}

void f1ap_du_impl::handle_ue_context_setup_request(const asn1::f1ap::ue_context_setup_request_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id->value);
  f1ap_du_ue*         u                 = ues.find(gnb_du_ue_f1ap_id);

  if (u == nullptr) {
    logger.error("Discarding UeContextSetupRequest cause=Unrecognized gNB-DU UE F1AP ID={}", gnb_du_ue_f1ap_id);
    // TODO: Handle.
    return;
  }

  u->handle_ue_context_setup_request(msg);
}

void f1ap_du_impl::handle_ue_context_release_command(const asn1::f1ap::ue_context_release_cmd_s& msg)
{
  if (msg->old_gnb_du_ue_f1ap_id_present) {
    // If the old gNB-DU UE F1AP ID IE is included in the UE CONTEXT RELEASE COMMAND message, the gNB-DU shall
    // additionally release the UE context associated with the old gNB-DU UE F1AP ID.
    gnb_du_ue_f1ap_id_t old_gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->old_gnb_du_ue_f1ap_id->value);
    f1ap_du_ue*         old_ue                = ues.find(old_gnb_du_ue_f1ap_id);
    if (old_ue != nullptr) {
      du_mng.get_ue_handler(old_ue->context.ue_index)
          .schedule_async_task(du_mng.request_ue_removal(f1ap_ue_delete_request{old_ue->context.ue_index}));
    } else {
      logger.warning("old gNB-DU UE F1AP ID={} not found", old_gnb_du_ue_f1ap_id);
    }
  }

  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id->value);
  f1ap_du_ue*         u                 = ues.find(gnb_du_ue_f1ap_id);
  if (u == nullptr) {
    logger.warning("Discarding UeContextReleaseCommand cause=Unrecognized gNB-DU UE F1AP ID={}", gnb_du_ue_f1ap_id);
    return;
  }

  du_mng.get_ue_handler(u->context.ue_index).schedule_async_task(launch_async<f1ap_du_ue_release_procedure>(msg, *u));
}

void f1ap_du_impl::handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id->value);
  f1ap_du_ue*         ue                = ues.find(gnb_du_ue_f1ap_id);

  if (ue == nullptr) {
    logger.error("Discarding UeContextModificationRequest cause=Unrecognized gNB-DU UE F1AP ID={}", gnb_du_ue_f1ap_id);
    // TODO: Handle.
    return;
  }

  ue->handle_ue_context_modification_request(msg);
}

void f1ap_du_impl::handle_dl_rrc_message_transfer(const asn1::f1ap::dl_rrc_msg_transfer_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id->value);

  // [TS38.473, 8.4.2.2.] If a UE-associated logical F1-connection exists, the DL RRC MESSAGE TRANSFER message shall
  // contain the gNBDU UE F1AP ID IE, which should be used by gNB-DU to lookup the stored UE context.
  f1ap_du_ue* ue = ues.find(gnb_du_ue_f1ap_id);

  if (ue == nullptr) {
    // [TS38.473, 8.4.2.2.] If no UE-associated logical F1-connection exists, the UE-associated logical F1-connection
    // shall be established at reception of the DL RRC MESSAGE TRANSFER message.
    // TODO.
    return;
  }

  // Handle gNB-CU UE F1AP ID.
  if (not handle_rx_message_gnb_cu_ue_f1ap_id(*ue, int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id->value))) {
    return;
  }

  if (msg->new_gnb_cu_ue_f1ap_id_present) {
    // If the DL RRC MESSAGE TRANSFER message contains the New gNB-CU UE F1AP ID IE, the gNB-DU shall, if supported,
    // replace the value received in the gNB-CU UE F1AP ID IE by the value of the New gNB-CU UE F1AP ID and use it for
    // further signalling.
    ue->context.gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(msg->new_gnb_cu_ue_f1ap_id.value);
  }

  // TODO: Check old gNB-DU UE F1AP ID.

  if (msg->old_gnb_du_ue_f1ap_id_present) {
    // > If the gNB-DU identifies the UE-associated logical F1-connection by the gNB-DU UE F1AP ID IE in the
    // DL RRC MESSAGE TRANSFER message and the old gNB-DU UE F1AP ID IE is included, it shall release the old gNB-DU
    // UE F1AP ID and the related configurations associated with the old gNB-DU UE F1AP ID.
    gnb_du_ue_f1ap_id_t old_gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->old_gnb_du_ue_f1ap_id->value);
    f1ap_du_ue*         old_ue                = ues.find(old_gnb_du_ue_f1ap_id);
    if (old_ue != nullptr) {
      f1ap_ue_delete_request request{};
      request.ue_index = old_ue->context.ue_index;
      du_mng.get_ue_handler(old_ue->context.ue_index).schedule_async_task(du_mng.request_ue_removal(request));
    } else {
      logger.warning("old gNB-DU UE F1AP ID={} not found", old_gnb_du_ue_f1ap_id);
    }
  }

  srb_id_t    srb_id     = int_to_srb_id(msg->srb_id->value);
  f1c_bearer* srb_bearer = ue->bearers.find_srb(srb_id);
  if (srb_bearer == nullptr) {
    logger.warning("Discarding DlRrcMessageTransfer cause=SRB-ID={} not found", srb_id);
    // TODO: Handle error.
    return;
  }

  // Forward SDU to lower layers.
  byte_buffer sdu;
  sdu.append(msg->rrc_container.value);
  ue_exec_mapper.executor(ue->context.ue_index).execute([sdu = std::move(sdu), srb_bearer]() mutable {
    srb_bearer->handle_pdu(std::move(sdu));
  });
}

async_task<f1ap_ue_context_modification_response_message>
f1ap_du_impl::handle_ue_context_modification_required(const f1ap_ue_context_modification_required_message& msg)
{
  // TODO: add procedure implementation

  f1ap_event_manager::f1ap_ue_context_modification_outcome_t ue_ctxt_mod_resp;

  return launch_async([this, ue_ctxt_mod_resp, res = f1ap_ue_context_modification_response_message{}, msg](
                          coro_context<async_task<f1ap_ue_context_modification_response_message>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(ue_ctxt_mod_resp, events->f1ap_ue_context_modification_response);

    if (ue_ctxt_mod_resp.has_value()) {
      logger.debug("Received PDU with successful outcome");
      res.confirm = *ue_ctxt_mod_resp.value();
      res.success = true;
    } else {
      logger.debug("Received PDU with unsuccessful outcome");
      res.refuse  = *ue_ctxt_mod_resp.error();
      res.success = false;
    }

    CORO_RETURN(res);
  });
}

void f1ap_du_impl::handle_message(const f1ap_message& msg)
{
  // Log message.
  expected<gnb_du_ue_f1ap_id_t> gnb_du_ue_f1ap_id = get_gnb_du_ue_f1ap_id(msg.pdu);
  expected<uint8_t>             transaction_id    = get_transaction_id(msg.pdu);
  if (transaction_id.has_value()) {
    logger.debug("SDU \"{}::{}\" transaction id={}",
                 msg.pdu.type().to_string(),
                 get_message_type_str(msg.pdu),
                 transaction_id.value());
  } else if (gnb_du_ue_f1ap_id.has_value()) {
    logger.debug("SDU \"{}::{}\" GNB-DU-UE-F1AP-ID={}",
                 msg.pdu.type().to_string(),
                 get_message_type_str(msg.pdu),
                 gnb_du_ue_f1ap_id.value());
  }

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    msg.pdu.to_json(js);
    logger.debug("Containerized PDU: {}", js.to_string());
  }

  // Run F1AP protocols in Control executor.
  ctrl_exec.execute([this, msg]() {
    switch (msg.pdu.type().value) {
      case asn1::f1ap::f1ap_pdu_c::types_opts::init_msg:
        handle_initiating_message(msg.pdu.init_msg());
        break;
      case asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome:
        handle_successful_outcome(msg.pdu.successful_outcome());
        break;
      case asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome:
        handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
        break;
      default:
        logger.error("Invalid PDU type");
        break;
    }
  });
}

void f1ap_du_impl::handle_initiating_message(const asn1::f1ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cfg_upd:
      handle_gnb_cu_configuration_update(msg.value.gnb_cu_cfg_upd());
      break;
    case f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer:
      handle_dl_rrc_message_transfer(msg.value.dl_rrc_msg_transfer());
      break;
    case f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request:
      handle_ue_context_setup_request(msg.value.ue_context_setup_request());
      break;
    case f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request:
      handle_ue_context_modification_request(msg.value.ue_context_mod_request());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void f1ap_du_impl::handle_successful_outcome(const asn1::f1ap::successful_outcome_s& outcome)
{
  expected<uint8_t> transaction_id = get_transaction_id(outcome);
  if (transaction_id.is_error()) {
    logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
    return;
  }

  // Set transaction result and resume suspended procedure.
  if (not events->transactions.set(transaction_id.value(), outcome)) {
    logger.warning("Unexpected transaction id={}", transaction_id.value());
  }
}

void f1ap_du_impl::handle_unsuccessful_outcome(const asn1::f1ap::unsuccessful_outcome_s& outcome)
{
  expected<uint8_t> transaction_id = get_transaction_id(outcome);
  if (transaction_id.is_error()) {
    logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
    return;
  }

  // Set transaction result and resume suspended procedure.
  if (not events->transactions.set(transaction_id.value(), outcome)) {
    logger.warning("Unexpected transaction id={}", transaction_id.value());
  }
}

bool f1ap_du_impl::handle_rx_message_gnb_cu_ue_f1ap_id(f1ap_du_ue& ue, gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id)
{
  if (ue.context.gnb_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id) {
    // No update in gNB-CU UE F1AP ID.
    return true;
  }

  // [TS38.473, Cause IE] The action failed because the gNB-CU UE F1AP ID is either unknown, or (for a first
  // message received at the gNB-CU) is known and already allocated to an existing context.
  const f1ap_du_ue* ue_cu_id = ues.find(gnb_cu_ue_f1ap_id);
  if (ue.context.gnb_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid and ue_cu_id == nullptr) {
    // First message from the gNB-CU for this UE. Update gNB-CU-UE-F1AP-ID.
    ue.context.gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id;
    return true;
  }
  cause_c cause;
  cause.set_radio_network().value = cause_radio_network_opts::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  if (ue_cu_id == nullptr) {
    logger.warning("Discarding message cause=gNB-CU UE F1AP ID={} is unknown");
  } else {
    logger.warning("Discarding message cause=gNB-CU UE F1AP ID={} is known and already allocated to an existing "
                   "context with gNB-DU UE F1AP ID={}",
                   gnb_cu_ue_f1ap_id,
                   ue_cu_id->context.gnb_du_ue_f1ap_id);
  }
  send_error_indication(cause);
  return false;
}

void f1ap_du_impl::send_error_indication(const asn1::f1ap::cause_c& cause)
{
  // TODO
}
