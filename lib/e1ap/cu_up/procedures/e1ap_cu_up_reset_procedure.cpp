/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_cu_up_reset_procedure.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include <utility>

using namespace srsran;
using namespace srs_cu_up;

e1ap_cu_up_reset_procedure::e1ap_cu_up_reset_procedure(asn1::e1ap::reset_s          reset_msg_,
                                                       e1ap_ue_context_list&        ue_ctxt_list_,
                                                       e1ap_cu_up_manager_notifier& cu_up_notifier_,
                                                       e1ap_message_notifier&       tx_pdu_notifier_,
                                                       srslog::basic_logger&        logger_) :
  reset_msg(std::move(reset_msg_)),
  ue_ctxt_list(ue_ctxt_list_),
  cu_up_notifier(cu_up_notifier_),
  tx_pdu_notifier(tx_pdu_notifier_),
  logger(logger_)
{
}

void e1ap_cu_up_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  // Release CU-UP Bearer contexts.
  if (reset_msg->reset_type.type() == asn1::e1ap::reset_type_c::types_opts::e1_interface) {
    CORO_AWAIT(handle_e1_interface_reset(reset_msg->reset_type.e1_interface()));
  } else if (reset_msg->reset_type.type() == asn1::e1ap::reset_type_c::types_opts::part_of_e1_interface) {
    CORO_AWAIT(handle_part_of_e1_interface_reset(reset_msg->reset_type.part_of_e1_interface()));
  } else {
    CORO_EARLY_RETURN();
  }

  generate_e1ap_reset_ack();
  CORO_RETURN();
}

async_task<void> e1ap_cu_up_reset_procedure::handle_e1_interface_reset(const asn1::e1ap::reset_all_e& reset_all)
{
  return cu_up_notifier.on_e1_reset_received({e1ap_reset::full, {}});
}

async_task<void> e1ap_cu_up_reset_procedure::handle_part_of_e1_interface_reset(
    const asn1::e1ap::ue_associated_lc_e1_conn_list_res_l& ue_reset_list)
{
  std::vector<ue_index_t> ues;
  for (const auto& ue : ue_reset_list) {
    if (ue->type() !=
        asn1::e1ap::ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::ue_associated_lc_e1_conn_item) {
      continue;
    }
    asn1::e1ap::ue_associated_lc_e1_conn_item_s lc = ue->ue_associated_lc_e1_conn_item();
    if (not lc.gnb_cu_up_ue_e1ap_id_present) {
      continue;
    }

    gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(lc.gnb_cu_up_ue_e1ap_id);
    if (cu_up_e1ap_id == gnb_cu_up_ue_e1ap_id_t::invalid) {
      continue;
    }

    const e1ap_ue_context* ue_ctx = ue_ctxt_list.find_ue(cu_up_e1ap_id);
    if (ue_ctx == nullptr) {
      continue;
    }

    ues.push_back(ue_ctx->ue_ids.ue_index);
  }
  return cu_up_notifier.on_e1_reset_received({e1ap_reset::partial, ues});
}

void e1ap_cu_up_reset_procedure::generate_e1ap_reset_ack()
{
  e1ap_message e1ap_msg;
  e1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_E1AP_ID_RESET);
  asn1::e1ap::reset_ack_s& e1_reset_ack = e1ap_msg.pdu.successful_outcome().value.reset_ack();
  e1_reset_ack->transaction_id          = reset_msg->transaction_id;

  tx_pdu_notifier.on_new_message(e1ap_msg);
}
