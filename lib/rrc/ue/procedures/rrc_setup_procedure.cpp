/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_setup_procedure.h"
#include "../rrc_asn1_helpers.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_setup_procedure::rrc_setup_procedure(rrc_ue_context_t&                          context_,
                                         const asn1::rrc_nr::establishment_cause_e& cause_,
                                         const byte_buffer&                         du_to_cu_container_,
                                         rrc_ue_setup_proc_notifier&                rrc_ue_notifier_,
                                         rrc_ue_srb_handler&                        srb_notifier_,
                                         rrc_ue_nas_notifier&                       nas_notifier_,
                                         rrc_ue_event_manager&                      event_mng_,
                                         rrc_ue_logger&                             logger_) :
  context(context_),
  cause(cause_),
  du_to_cu_container(du_to_cu_container_),
  rrc_ue(rrc_ue_notifier_),
  srb_notifier(srb_notifier_),
  nas_notifier(nas_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
}

void rrc_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // create SRB1
  create_srb1();

  // create new transaction for RRCSetup
  transaction =
      event_mng.transactions.create_transaction(std::chrono::milliseconds(context.cfg.rrc_procedure_timeout_ms));

  // send RRC setup to UE
  send_rrc_setup();

  // Await UE response
  CORO_AWAIT(transaction);

  if (transaction.has_response()) {
    logger.log_debug("\"{}\" finished successfully", name());
    context.state = rrc_state::connected;
    send_initial_ue_msg(transaction.response().msg.c1().rrc_setup_complete());
  } else if (transaction.failure_cause() == protocol_transaction_failure::timeout) {
    logger.log_warning("\"{}\" timed out after {}ms", name(), context.cfg.rrc_procedure_timeout_ms);
    rrc_ue.on_ue_release_required(cause_protocol_t::unspecified);
  } else {
    logger.log_warning("\"{}\" cancelled", name());
    // Do nothing. We are likely shutting down the DU processor.
  }

  CORO_RETURN();
}

void rrc_setup_procedure::create_srb1()
{
  // create SRB1
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index = context.ue_index;
  srb1_msg.srb_id   = srb_id_t::srb1;
  srb1_msg.pdcp_cfg = {};
  srb_notifier.create_srb(srb1_msg);
}

void rrc_setup_procedure::send_rrc_setup()
{
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_setup();
  rrc_setup_s& rrc_setup = dl_ccch_msg.msg.c1().rrc_setup();
  if (!fill_asn1_rrc_setup_msg(rrc_setup, du_to_cu_container, transaction.id())) {
    logger.log_warning("ASN1 RRC setup message fill failed");
    return;
  }
  rrc_ue.on_new_dl_ccch(dl_ccch_msg);
}

void rrc_setup_procedure::send_initial_ue_msg(const asn1::rrc_nr::rrc_setup_complete_s& rrc_setup_complete_msg)
{
  cu_cp_initial_ue_message init_ue_msg       = {};
  init_ue_msg.ue_index                       = context.ue_index;
  init_ue_msg.nas_pdu                        = rrc_setup_complete_msg.crit_exts.rrc_setup_complete().ded_nas_msg.copy();
  init_ue_msg.establishment_cause            = static_cast<establishment_cause_t>(cause.value);
  init_ue_msg.user_location_info.nr_cgi      = context.cell.cgi;
  init_ue_msg.user_location_info.tai.plmn_id = context.cell.cgi.plmn_id;
  init_ue_msg.user_location_info.tai.tac     = context.cell.tac;

  cu_cp_five_g_s_tmsi five_g_s_tmsi;
  if (context.five_g_tmsi.has_value()) {
    five_g_s_tmsi.five_g_tmsi = context.five_g_tmsi.value();
    // amf_pointer and amf_set_id will be set by NGAP
    init_ue_msg.five_g_s_tmsi = five_g_s_tmsi;
  }

  if (rrc_setup_complete_msg.crit_exts.rrc_setup_complete().registered_amf_present) {
    cu_cp_amf_identifier_t amf_id =
        asn1_to_amf_identifier(rrc_setup_complete_msg.crit_exts.rrc_setup_complete().registered_amf.amf_id);

    init_ue_msg.amf_set_id = amf_id.amf_set_id;
    // TODO: Handle PLMN ID
  }

  nas_notifier.on_initial_ue_message(init_ue_msg);
}
