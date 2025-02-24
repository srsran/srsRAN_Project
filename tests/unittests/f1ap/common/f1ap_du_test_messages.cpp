/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_test_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace asn1::f1ap;

f1ap_message srsran::generate_f1ap_dl_rrc_message_transfer(srb_id_t srb_id, const byte_buffer& rrc_container)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
  auto& dl_msg              = msg.pdu.init_msg().value.dl_rrc_msg_transfer();
  dl_msg->gnb_cu_ue_f1ap_id = 0;
  dl_msg->gnb_du_ue_f1ap_id = 0;
  dl_msg->srb_id            = srb_id_to_uint(srb_id);
  dl_msg->rrc_container     = rrc_container.deep_copy().value();

  return msg;
}
