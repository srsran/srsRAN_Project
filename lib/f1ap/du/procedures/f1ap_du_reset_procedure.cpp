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

#include "f1ap_du_reset_procedure.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

reset_procedure::reset_procedure(const asn1::f1ap::reset_s& msg_,
                                 f1ap_du_configurator&      du_mng_,
                                 f1ap_du_ue_manager&        ue_mng_,
                                 f1ap_message_notifier&     msg_notifier_) :
  msg(msg_), du_mng(du_mng_), ue_mng(ue_mng_), msg_notifier(msg_notifier_), logger(srslog::fetch_basic_logger("DU-F1"))
{
}

void reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("{}: Started with cause=\"{}\".", name(), msg->cause.type().to_string());

  CORO_AWAIT(handle_reset());

  send_ack();

  logger.debug("{}: Finished successfully.", name());

  CORO_RETURN();
}

async_task<void> reset_procedure::handle_reset()
{
  using reset_choice = asn1::f1ap::reset_type_c;

  switch (msg->reset_type.type().value) {
    case reset_choice::types_opts::f1_interface:
      // Request DU to remove all UEs.
      if (ue_mng.nof_ues() != 0) {
        return du_mng.request_reset({});
      }
      break;
    case reset_choice::types_opts::part_of_f1_interface: {
      // Request DU to remove a subset of UEs.
      std::vector<du_ue_index_t> ues_to_rem = create_ues_to_reset();
      if (not ues_to_rem.empty()) {
        return du_mng.request_reset(ues_to_rem);
      }
    } break;
    case reset_choice::types_opts::choice_ext:
      logger.warning("Ignoring RESET message. Cause: choiceExt option not supported");
      break;
    default:
      report_fatal_error("Invalid F1 reset type");
  }

  return launch_no_op_task();
}

std::vector<du_ue_index_t> reset_procedure::create_ues_to_reset() const
{
  using namespace asn1::f1ap;

  const ue_associated_lc_f1_conn_list_res_l& in_list = msg->reset_type.part_of_f1_interface();

  std::vector<du_ue_index_t> ues_to_reset;
  ues_to_reset.reserve(in_list.size());
  for (const auto& elem : in_list) {
    const auto& conn_item = elem->ue_associated_lc_f1_conn_item();

    if (conn_item.gnb_du_ue_f1ap_id_present) {
      const gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(conn_item.gnb_du_ue_f1ap_id);
      auto*                     u        = ue_mng.find(du_ue_id);
      if (u != nullptr) {
        ues_to_reset.push_back(u->context.ue_index);
        continue;
      }
    }

    if (conn_item.gnb_cu_ue_f1ap_id_present) {
      const gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(conn_item.gnb_cu_ue_f1ap_id);
      auto*                     u        = ue_mng.find(cu_ue_id);
      if (u != nullptr) {
        ues_to_reset.push_back(u->context.ue_index);
        continue;
      }
    }

    // UE could not be found. We still include it in the RESET ACK, as per TS 38.473.
    // "The gNB-DU shall include in the RESET ACKNOWLEDGE message, for each UE association to be reset, the
    // UE-associated logical F1-connection Item IE in the UE-associated logical F1-connection list IE. The UE-associated
    // logical F1-connection Item IEs shall be in the same order as received in the RESET message and shall include
    // also unknown UE-associated logical F1-connections."
  }

  return ues_to_reset;
}

void reset_procedure::send_ack() const
{
  using namespace asn1::f1ap;

  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_RESET);
  reset_ack_s& ack = resp.pdu.successful_outcome().value.reset_ack();

  ack->transaction_id = msg->transaction_id;

  // Fill list of reset ack UEs.
  if (msg->reset_type.type().value == reset_type_c::types_opts::part_of_f1_interface) {
    auto& req_list                                     = msg->reset_type.part_of_f1_interface();
    ack->ue_associated_lc_f1_conn_list_res_ack_present = true;
    ack->ue_associated_lc_f1_conn_list_res_ack.resize(req_list.size());
    for (unsigned i = 0; i != req_list.size(); ++i) {
      ack->ue_associated_lc_f1_conn_list_res_ack[i].load_info_obj(ASN1_F1AP_ID_UE_ASSOCIATED_LC_F1_CONN_ITEM);
      auto& item = ack->ue_associated_lc_f1_conn_list_res_ack[i].value().ue_associated_lc_f1_conn_item();
      item       = req_list[i]->ue_associated_lc_f1_conn_item();
    }
  }

  msg_notifier.on_new_message(resp);
}
