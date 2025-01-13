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

#include "f1ap_cu_ue_context.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

void f1ap_ue_context::handle_dl_rrc_message(const f1ap_dl_rrc_message& msg, f1ap_message_notifier& msg_notifier)
{
  srsran_sanity_check(ue_ids.du_ue_f1ap_id && ue_ids.du_ue_f1ap_id != gnb_du_ue_f1ap_id_t::invalid,
                      "Invalid gNB-DU-UE-F1AP-Id");

  asn1::f1ap::dl_rrc_msg_transfer_s dl_rrc_msg = {};
  dl_rrc_msg->gnb_cu_ue_f1ap_id                = gnb_cu_ue_f1ap_id_to_uint(ue_ids.cu_ue_f1ap_id);
  dl_rrc_msg->gnb_du_ue_f1ap_id                = gnb_du_ue_f1ap_id_to_uint(*ue_ids.du_ue_f1ap_id);
  dl_rrc_msg->srb_id                           = static_cast<uint8_t>(srb_id_to_uint(msg.srb_id));
  dl_rrc_msg->rrc_container                    = msg.rrc_container.copy();

  if (pending_old_ue_id.has_value()) {
    // If the UE requests to reestablish RRC connection in the last serving gNB-DU, the DL RRC MESSAGE TRANSFER message
    // shall include old gNB-DU UE F1AP ID, see TS 38.401 section 8.7.
    dl_rrc_msg->old_gnb_du_ue_f1ap_id_present = true;
    dl_rrc_msg->old_gnb_du_ue_f1ap_id         = gnb_du_ue_f1ap_id_to_uint(pending_old_ue_id.value());

    pending_old_ue_id.reset();
  }

  // Pack message into PDU.
  f1ap_message f1ap_dl_rrc_msg;
  f1ap_dl_rrc_msg.pdu.set_init_msg();
  f1ap_dl_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
  f1ap_dl_rrc_msg.pdu.init_msg().value.dl_rrc_msg_transfer() = std::move(dl_rrc_msg);

  // Send DL RRC message.
  msg_notifier.on_new_message(f1ap_dl_rrc_msg);
}
