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
#include "du/procedures/f1ap_du_setup_procedure.h"
#include "du/procedures/f1ap_du_ue_creation_procedure.h"
#include "du/procedures/f1ap_du_ue_release_procedure.h"
#include "du/procedures/gnb_cu_configuration_update_procedure.h"
#include "f1c_du_bearer_impl.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/support/async/event_signal.h"

using namespace srsgnb;
using namespace asn1::f1ap;
using namespace srs_du;

f1ap_du_impl::f1ap_du_impl(f1c_message_notifier&       message_notifier_,
                           f1c_du_configurator&        du_mng_,
                           task_executor&              ctrl_exec_,
                           du_high_ue_executor_mapper& ue_exec_mapper_) :
  logger(srslog::fetch_basic_logger("DU-F1AP")),
  f1c_notifier(message_notifier_),
  ctrl_exec(ctrl_exec_),
  ue_exec_mapper(ue_exec_mapper_),
  du_mng(du_mng_),
  ues(du_mng_, f1c_notifier),
  events(std::make_unique<f1ap_event_manager>(du_mng.get_timer_manager()))
{
  (void)ctrl_exec;
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
f1ap_du_impl::~f1ap_du_impl() {}

async_task<f1_setup_response_message> f1ap_du_impl::handle_f1ap_setup_request(const f1_setup_request_message& request)
{
  return launch_async<f1ap_du_setup_procedure>(request, f1c_notifier, *events, du_mng.get_timer_manager(), ctxt);
}

f1ap_ue_create_response f1ap_du_impl::handle_ue_creation_request(const f1ap_ue_create_request& msg)
{
  return create_f1ap_du_ue(msg, ues, ctxt, f1c_notifier, *events);
}

void f1ap_du_impl::handle_gnb_cu_configuration_update(const asn1::f1ap::gnbcu_cfg_upd_s& msg)
{
  du_mng.schedule_async_task(launch_async<gnb_cu_configuration_update_procedure>(msg, f1c_notifier));
}

void f1ap_du_impl::handle_ue_context_setup_request(const asn1::f1ap::ue_context_setup_request_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1_ap_id->value);
  f1ap_du_ue*         u                 = ues.find(gnb_du_ue_f1ap_id);

  if (u == nullptr) {
    logger.error("Discarding UE CONTEXT SETUP REQUEST. Cause: Unrecognized gNB-DU UE F1AP ID={}", gnb_du_ue_f1ap_id);
    // TODO: Handle.
    return;
  }

  u->handle_ue_context_setup_request(msg);
}

void f1ap_du_impl::handle_ue_context_release_command(const asn1::f1ap::ue_context_release_cmd_s& msg)
{
  if (msg->oldg_nb_du_ue_f1_ap_id_present) {
    // If the old gNB-DU UE F1AP ID IE is included in the UE CONTEXT RELEASE COMMAND message, the gNB-DU shall
    // additionally release the UE context associated with the old gNB-DU UE F1AP ID.
    gnb_du_ue_f1ap_id_t old_gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->oldg_nb_du_ue_f1_ap_id->value);
    f1ap_du_ue*         old_ue                = ues.find(old_gnb_du_ue_f1ap_id);
    if (old_ue != nullptr) {
      du_mng.get_ue_handler(old_ue->context.ue_index)
          .schedule_async_task(du_mng.request_ue_removal(f1ap_ue_delete_request{old_ue->context.ue_index}));
    } else {
      logger.warning("old gNB-DU UE F1AP ID={} not found", old_gnb_du_ue_f1ap_id);
    }
  }

  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1_ap_id->value);
  f1ap_du_ue*         u                 = ues.find(gnb_du_ue_f1ap_id);
  if (u == nullptr) {
    logger.warning("Discarding UE CONTEXT RELEASE COMMAND. Cause: Unrecognized gNB-DU UE F1AP ID={}",
                   gnb_du_ue_f1ap_id);
    return;
  }

  du_mng.get_ue_handler(u->context.ue_index).schedule_async_task(launch_async<f1ap_du_ue_release_procedure>(msg, *u));
}

void f1ap_du_impl::handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1_ap_id->value);
  f1ap_du_ue*         ue                = ues.find(gnb_du_ue_f1ap_id);

  if (ue == nullptr) {
    logger.error("Discarding UE CONTEXT MODIFICATION REQUEST. Cause: Unrecognized gNB-DU UE F1AP ID={}",
                 gnb_du_ue_f1ap_id);
    // TODO: Handle.
    return;
  }

  ue->handle_ue_context_modification_request(msg);
}

void f1ap_du_impl::handle_dl_rrc_message_transfer(const asn1::f1ap::dlrrc_msg_transfer_s& msg)
{
  gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1_ap_id->value);
  f1ap_du_ue*         ue                = ues.find(gnb_du_ue_f1ap_id);

  // TODO: Check old gNB-DU UE F1AP ID.

  if (msg->oldg_nb_du_ue_f1_ap_id_present) {
    // > If the gNB-DU identifies the UE-associated logical F1-connection by the gNB-DU UE F1AP ID IE in the
    // DL RRC MESSAGE TRANSFER message and the old gNB-DU UE F1AP ID IE is included, it shall release the old gNB-DU
    // UE F1AP ID and the related configurations associated with the old gNB-DU UE F1AP ID.
    gnb_du_ue_f1ap_id_t old_gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->oldg_nb_du_ue_f1_ap_id->value);
    f1ap_du_ue*         old_ue                = ues.find(old_gnb_du_ue_f1ap_id);
    if (old_ue != nullptr) {
      f1ap_ue_delete_request request{};
      request.ue_index = old_ue->context.ue_index;
      du_mng.get_ue_handler(old_ue->context.ue_index).schedule_async_task(du_mng.request_ue_removal(request));
    } else {
      logger.warning("old gNB-DU UE F1AP ID={} not found", old_gnb_du_ue_f1ap_id);
    }
  }

  if (msg->new_g_nb_cu_ue_f1_ap_id_present) {
    // > If the DL RRC MESSAGE TRANSFER message contains the New gNB-CU UE F1AP ID IE, the gNB-DU shall, if supported,
    // replace the value received in the gNB-CU UE F1AP ID IE by the value of the New gNB-CU UE F1AP ID and use it
    // for further signalling.
    ue->context.gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(msg->new_g_nb_cu_ue_f1_ap_id->value);
  }

  srb_id_t   srb_id     = int_to_srb_id(msg->srbid->value);
  f1_bearer* srb_bearer = ue->find_srb(srb_id);
  if (srb_bearer == nullptr) {
    logger.warning("Discarding DL RRC Message Transfer. Cause: SRB Id={} not found", srb_id);
    // TODO: Handle error.
    return;
  }

  // Forward SDU to lower layers.
  byte_buffer sdu;
  sdu.append(msg->rrc_container.value);
  ue_exec_mapper.executor(ue->context.ue_index).execute([sdu = std::move(sdu), srb_bearer]() mutable {
    srb_bearer->handle_sdu(std::move(sdu));
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
      logger.info("Received F1AP PDU with successful outcome.");
      res.confirm = *ue_ctxt_mod_resp.value();
      res.success = true;
    } else {
      logger.info("Received F1AP PDU with unsuccessful outcome.");
      res.refuse  = *ue_ctxt_mod_resp.error();
      res.success = false;
    }

    CORO_RETURN(res);
  });
}

void f1ap_du_impl::handle_message(const f1c_message& msg)
{
  // Log message.
  expected<gnb_du_ue_f1ap_id_t> gnb_du_ue_f1ap_id = get_gnb_du_ue_f1ap_id(msg.pdu);
  expected<uint8_t>             transaction_id    = get_transaction_id(msg.pdu);
  if (transaction_id.has_value()) {
    logger.info("F1AP SDU, \"{}.{}\", transaction id={}",
                msg.pdu.type().to_string(),
                get_message_type_str(msg.pdu),
                transaction_id.value());
  } else if (gnb_du_ue_f1ap_id.has_value()) {
    logger.info("F1AP SDU, \"{}.{}\", GNB-DU-UE-F1AP-ID={}", msg.pdu.type().to_string(), gnb_du_ue_f1ap_id.value());
  } else {
    logger.info("F1AP SDU, \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));
  }

  switch (msg.pdu.type().value) {
    case asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg:
      handle_initiating_message(msg.pdu.init_msg());
      break;
    case asn1::f1ap::f1_ap_pdu_c::types_opts::successful_outcome:
      handle_successful_outcome(msg.pdu.successful_outcome());
      break;
    case asn1::f1ap::f1_ap_pdu_c::types_opts::unsuccessful_outcome:
      handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
      break;
    default:
      logger.error("Invalid PDU type");
      break;
  }
}

void f1ap_du_impl::handle_initiating_message(const asn1::f1ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::gnbcu_cfg_upd:
      handle_gnb_cu_configuration_update(msg.value.gnbcu_cfg_upd());
      break;
    case f1_ap_elem_procs_o::init_msg_c::types_opts::dlrrc_msg_transfer:
      handle_dl_rrc_message_transfer(msg.value.dlrrc_msg_transfer());
      break;
    case f1_ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request:
      handle_ue_context_setup_request(msg.value.ue_context_setup_request());
      break;
    case f1_ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request:
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
  events->transactions.set(transaction_id.value(), outcome);
}

void f1ap_du_impl::handle_unsuccessful_outcome(const asn1::f1ap::unsuccessful_outcome_s& outcome)
{
  expected<uint8_t> transaction_id = get_transaction_id(outcome);
  if (transaction_id.is_error()) {
    logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
    return;
  }

  // Set transaction result and resume suspended procedure.
  events->transactions.set(transaction_id.value(), outcome);
}
