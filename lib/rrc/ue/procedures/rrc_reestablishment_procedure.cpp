/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_reestablishment_procedure.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_reestablishment_procedure::rrc_reestablishment_procedure(rrc_ue_context_t&                     context_,
                                                             const ue_index_t                      old_ue_index_,
                                                             rrc_ue_reestablishment_proc_notifier& rrc_ue_notifier_,
                                                             rrc_ue_du_processor_notifier&    du_processor_notifier_,
                                                             rrc_ue_reestablishment_notifier& cu_cp_notifier_,
                                                             rrc_ue_control_notifier&         ngap_ctrl_notifier_,
                                                             rrc_ue_event_manager&            event_mng_,
                                                             srslog::basic_logger&            logger_) :
  context(context_),
  old_ue_index(old_ue_index_),
  rrc_ue(rrc_ue_notifier_),
  du_processor_notifier(du_processor_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  ngap_ctrl_notifier(ngap_ctrl_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
}

void rrc_reestablishment_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={} old_ue={}: \"{}\" initialized.", context.ue_index, old_ue_index, name());

  // create SRB1
  create_srb1();

  // create new transaction for RRC Reestablishment
  transaction = event_mng.transactions.create_transaction(rrc_reest_timeout_ms);

  // send RRC Reestablishment to UE
  send_rrc_reestablishment();

  // Notify CU-CP to transfer and remove UE contexts
  cu_cp_notifier.on_ue_transfer_required(context.ue_index, old_ue_index);

  // Await UE response
  CORO_AWAIT(transaction);

  if (transaction.result().has_value()) {
    context.state = rrc_state::connected;

    // Notify DU Processor to start a Reestablishment Context Modification Routine
    CORO_AWAIT_VALUE(context_modification_success,
                     du_processor_notifier.on_rrc_reestablishment_context_modification_required(context.ue_index));

    // trigger UE context release at AMF in case of failure
    if (not context_modification_success) {
      ue_context_release_request.ue_index = context.ue_index;
      ue_context_release_request.cause    = cause_t::radio_network;
      ngap_ctrl_notifier.on_ue_context_release_request(ue_context_release_request);

      logger.debug("ue={} old_ue={}: \"{}\" failed.", context.ue_index, old_ue_index, name());
    } else {
      logger.debug("ue={} old_ue={}: \"{}\" finalized.", context.ue_index, old_ue_index, name());
    }

  } else {
    logger.debug("ue={} \"{}\" timed out", context.ue_index, name());
    logger.debug("ue={} old_ue={}: \"{}\" failed.", context.ue_index, old_ue_index, name());
  }

  CORO_RETURN();
}

void rrc_reestablishment_procedure::create_srb1()
{
  // create SRB1
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index     = context.ue_index;
  srb1_msg.old_ue_index = old_ue_index;
  srb1_msg.srb_id       = srb_id_t::srb1;
  srb1_msg.pdcp_cfg     = srb1_pdcp_cfg;
  du_processor_notifier.on_create_srb(srb1_msg);

  // activate SRB1 PDCP security
  rrc_ue.on_new_as_security_context();
}

void rrc_reestablishment_procedure::send_rrc_reestablishment()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_reest();
  rrc_reest_s& rrc_reest       = dl_dcch_msg.msg.c1().rrc_reest();
  rrc_reest.rrc_transaction_id = transaction.id();
  rrc_reest.crit_exts.set_rrc_reest();

  // if the UE requests to reestablish RRC connection in the last serving gNB-DU, the DL RRC MESSAGE TRANSFER message
  // shall include old gNB-DU UE F1AP ID, see TS 38.401 section 8.7
  if (get_du_index_from_ue_index(old_ue_index) == get_du_index_from_ue_index(context.ue_index)) {
    rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg, old_ue_index);
  } else {
    rrc_ue.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
  }
}
