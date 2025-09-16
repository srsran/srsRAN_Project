/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_f1_reset_procedure.h"
#include "asn1_helpers.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_f1_reset_procedure::cu_cp_f1_reset_procedure(const f1ap_configuration& f1ap_cfg_,
                                                   const cu_cp_reset&        reset_,
                                                   f1ap_message_notifier&    pdu_notifier_,
                                                   f1ap_event_manager&       ev_mng_,
                                                   f1ap_ue_context_list&     ue_ctxt_list_,
                                                   srslog::basic_logger&     logger_) :
  f1ap_cfg(f1ap_cfg_),
  reset(reset_),
  pdu_notifier(pdu_notifier_),
  ev_mng(ev_mng_),
  ue_ctxt_list(ue_ctxt_list_),
  logger(logger_)
{
}

void cu_cp_f1_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  // Create transaction.
  transaction = ev_mng.transactions.create_transaction(f1ap_cfg.proc_timeout);

  // Forward message to DU.
  send_f1_reset();

  // Await DU response.
  CORO_AWAIT(transaction);

  if (transaction.aborted()) {
    logger.warning("\"{}\" failed. Cause: Timeout reached for F1ResetAcknowledge reception", name());
    CORO_EARLY_RETURN();
  }

  // TODO: Handle F1 RESET ACKNOWLEDGE message.

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}

void cu_cp_f1_reset_procedure::send_f1_reset()
{
  // Pack message into PDU.
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_RESET);
  asn1::f1ap::reset_s& f1_reset = f1ap_msg.pdu.init_msg().value.reset();
  f1_reset->transaction_id      = transaction.id();
  f1_reset->cause               = cause_to_asn1(std::get<f1ap_cause_t>(reset.cause));
  if (reset.interface_reset) {
    // Reset all UEs.
    f1_reset->reset_type.set_f1_interface() = asn1::f1ap::reset_all_opts::options::reset_all;
  } else {
    // Reset only specific UEs.
    asn1::f1ap::ue_associated_lc_f1_conn_list_res_l& reset_part_of_f1_interface =
        f1_reset->reset_type.set_part_of_f1_interface();
    for (auto& ue : reset.ues_to_reset) {
      if (!ue_ctxt_list.contains(ue)) {
        logger.warning("ue={}: Excluding UE from F1 Reset. UE context does not exist", ue);
      } else {
        auto& ue_ctxt = ue_ctxt_list[ue];

        asn1::protocol_ie_single_container_s<asn1::f1ap::ue_associated_lc_f1_conn_item_res_o> item_container;
        asn1::f1ap::ue_associated_lc_f1_conn_item_s& conn_item = item_container->ue_associated_lc_f1_conn_item();

        if (ue_ctxt.ue_ids.cu_ue_f1ap_id != gnb_cu_ue_f1ap_id_t::invalid) {
          conn_item.gnb_cu_ue_f1ap_id_present = true;
          conn_item.gnb_cu_ue_f1ap_id         = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.ue_ids.cu_ue_f1ap_id);
        }
        if (ue_ctxt.ue_ids.du_ue_f1ap_id.has_value()) {
          conn_item.gnb_du_ue_f1ap_id_present = true;
          conn_item.gnb_du_ue_f1ap_id         = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.ue_ids.du_ue_f1ap_id.value());
        }

        reset_part_of_f1_interface.push_back(item_container);
      }
    }
  }

  pdu_notifier.on_new_message(f1ap_msg);
}
