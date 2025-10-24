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

#include "cu_cp_e1_reset_procedure.h"
#include "common/e1ap_asn1_converters.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_e1_reset_procedure::cu_cp_e1_reset_procedure(const e1ap_configuration& e1ap_cfg_,
                                                   const cu_cp_reset&        reset_,
                                                   e1ap_message_notifier&    pdu_notifier_,
                                                   e1ap_transaction_manager& ev_mng_,
                                                   e1ap_ue_context_list&     ue_ctxt_list_,
                                                   srslog::basic_logger&     logger_) :
  e1ap_cfg(e1ap_cfg_),
  reset(reset_),
  pdu_notifier(pdu_notifier_),
  ev_mng(ev_mng_),
  ue_ctxt_list(ue_ctxt_list_),
  logger(logger_)
{
}

void cu_cp_e1_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  // Create transaction.
  transaction = ev_mng.transactions.create_transaction(e1ap_cfg.proc_timeout);

  // Forward message to CU-UP.
  if (not send_e1_reset()) {
    logger.debug("\"{}\" terminated early, as there are no UEs present at CU-UP", name());
    CORO_EARLY_RETURN();
  }

  // Await CU-UP response.
  CORO_AWAIT(transaction);

  if (transaction.aborted()) {
    logger.warning("\"{}\" failed. Cause: Timeout reached for E1ResetAcknowledge reception", name());
    CORO_EARLY_RETURN();
  }

  // TODO: Handle E1 RESET ACKNOWLEDGE message.

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN();
}

bool cu_cp_e1_reset_procedure::send_e1_reset()
{
  // Pack message into PDU.
  e1ap_message e1ap_msg;
  e1ap_msg.pdu.set_init_msg().load_info_obj(ASN1_E1AP_ID_RESET);
  asn1::e1ap::reset_s& e1_reset = e1ap_msg.pdu.init_msg().value.reset();
  e1_reset->transaction_id      = transaction.id();
  e1_reset->cause               = cause_to_asn1(std::get<e1ap_cause_t>(reset.cause));
  if (reset.interface_reset) {
    // Reset all UEs.
    e1_reset->reset_type.set_e1_interface() = asn1::e1ap::reset_all_opts::options::reset_all;

    // Send E1 Reset to CU-UP.
    pdu_notifier.on_new_message(e1ap_msg);
    return true;
  }

  // Reset only specific UEs.
  asn1::e1ap::ue_associated_lc_e1_conn_list_res_l& reset_part_of_e1_interface =
      e1_reset->reset_type.set_part_of_e1_interface();
  for (auto& ue : reset.ues_to_reset) {
    if (!ue_ctxt_list.contains(ue)) {
      logger.warning("ue={}: Excluding UE from E1 Reset. UE context does not exist", ue);
    } else {
      auto& ue_ctxt = ue_ctxt_list[ue];

      asn1::protocol_ie_single_container_s<asn1::e1ap::ue_associated_lc_e1_conn_item_res_o> item_container;
      asn1::e1ap::ue_associated_lc_e1_conn_item_s& conn_item = item_container->ue_associated_lc_e1_conn_item();

      if (ue_ctxt.ue_ids.cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid) {
        conn_item.gnb_cu_cp_ue_e1ap_id_present = true;
        conn_item.gnb_cu_cp_ue_e1ap_id         = gnb_cu_cp_ue_e1ap_id_to_uint(ue_ctxt.ue_ids.cu_cp_ue_e1ap_id);
      }
      if (ue_ctxt.ue_ids.cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid) {
        conn_item.gnb_cu_up_ue_e1ap_id_present = true;
        conn_item.gnb_cu_up_ue_e1ap_id         = gnb_cu_up_ue_e1ap_id_to_uint(ue_ctxt.ue_ids.cu_up_ue_e1ap_id);
      }

      reset_part_of_e1_interface.push_back(item_container);
    }
  }

  if (reset_part_of_e1_interface.size() == 0u) {
    // No E1 UEs to release. Skipping sending E1 reset.
    return false;
  }

  // Send the E1 Reset to CU-UP.
  pdu_notifier.on_new_message(e1ap_msg);
  return true;
}
