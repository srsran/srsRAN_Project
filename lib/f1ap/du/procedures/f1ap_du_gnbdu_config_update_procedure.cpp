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

#include "f1ap_du_gnbdu_config_update_procedure.h"
#include "../f1ap_asn1_converters.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_gnbdu_config_update_procedure::f1ap_du_gnbdu_config_update_procedure(
    const gnbdu_config_update_request& request_,
    f1ap_message_notifier&             cu_notif_,
    f1ap_event_manager&                ev_mng_) :
  request(request_), cu_notif(cu_notif_), ev_mng(ev_mng_), logger(srslog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_gnbdu_config_update_procedure::operator()(coro_context<async_task<gnbdu_config_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  transaction = ev_mng.transactions.create_transaction();

  // Send request to CU.
  send_gnbdu_cu_update_request();

  // Await CU response.
  CORO_AWAIT(transaction);

  // Handle transaction response.
  CORO_RETURN(generate_du_response());
}

void f1ap_du_gnbdu_config_update_procedure::send_gnbdu_cu_update_request()
{
  f1ap_message msg = {};
  // Set F1AP PDU contents.
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_GNB_DU_CFG_UPD);
  gnb_du_cfg_upd_s& upd = msg.pdu.init_msg().value.gnb_du_cfg_upd();

  upd->transaction_id = transaction.id();

  // servedCellsToModify
  upd->served_cells_to_modify_list_present = not request.cells_to_mod.empty();
  upd->served_cells_to_modify_list.resize(request.cells_to_mod.size());
  for (unsigned i = 0, e = request.cells_to_mod.size(); i != e; ++i) {
    const auto& celltomod = request.cells_to_mod[i];
    upd->served_cells_to_modify_list[i].load_info_obj(ASN1_F1AP_ID_SERVED_CELLS_TO_MODIFY_ITEM);
    served_cells_to_modify_item_s& asn1cell = upd->served_cells_to_modify_list[i].value().served_cells_to_modify_item();

    // set old NR-CGI.
    // Note: We do not yet support old NR CGI != NR CGI.
    asn1cell.old_nr_cgi.plmn_id = celltomod.cell_info.nr_cgi.plmn_id.to_bytes();
    asn1cell.old_nr_cgi.nr_cell_id.from_number(celltomod.cell_info.nr_cgi.nci.value());

    // set servedCellInfo.
    asn1cell.served_cell_info = make_asn1_served_cell_info(celltomod.cell_info, {});

    // set System Info.
    if (celltomod.du_sys_info.has_value()) {
      asn1cell.gnb_du_sys_info_present  = true;
      asn1cell.gnb_du_sys_info.mib_msg  = celltomod.du_sys_info->packed_mib.copy();
      asn1cell.gnb_du_sys_info.sib1_msg = celltomod.du_sys_info->packed_sib1.copy();
    }
  }

  // Send request to CU-CP.
  logger.info("F1 Setup: Sending gNB-DU Configuration Update to CU-CP...");
  cu_notif.on_new_message(msg);
}

gnbdu_config_update_response f1ap_du_gnbdu_config_update_procedure::generate_du_response() const
{
  gnbdu_config_update_response resp;
  resp.result = false;

  if (transaction.aborted()) {
    // Timeout or cancellation case.
    return resp;
  }

  const f1ap_transaction_response& cu_pdu_response = transaction.response();
  if (not cu_pdu_response.has_value()) {
    // Failure case.
    return resp;
  }

  if (cu_pdu_response.value().value.type().value !=
      f1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_du_cfg_upd_ack) {
    // Invalid response type.
    return resp;
  }

  // TODO: Handle remaining fields.

  resp.result = true;
  return resp;
}
