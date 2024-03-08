/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_test_message_validators.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;

bool srsran::test_helpers::is_init_ul_rrc_msg_transfer_valid(const f1ap_message& msg, rnti_t rnti)
{
  if (not(msg.pdu.type() == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg and
          msg.pdu.init_msg().proc_code == ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER)) {
    return false;
  }
  const asn1::f1ap::init_ul_rrc_msg_transfer_s& rrcmsg = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
  return rrcmsg->c_rnti == to_value(rnti);
}

bool srsran::test_helpers::is_ul_rrc_msg_transfer_valid(const f1ap_message& msg, srb_id_t srb_id)
{
  if (not(msg.pdu.type() == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg and
          msg.pdu.init_msg().proc_code == ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER)) {
    return false;
  }
  const asn1::f1ap::ul_rrc_msg_transfer_s& rrcmsg = msg.pdu.init_msg().value.ul_rrc_msg_transfer();
  if (rrcmsg->srb_id != srb_id_to_uint(srb_id) or rrcmsg->rrc_container.empty()) {
    return false;
  }
  return true;
}

bool srsran::test_helpers::is_ue_context_setup_response_valid(const f1ap_message& msg)
{
  if (not(msg.pdu.type() == asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome and
          msg.pdu.successful_outcome().proc_code == ASN1_F1AP_ID_UE_CONTEXT_SETUP)) {
    return false;
  }
  const asn1::f1ap::ue_context_setup_resp_s& resp = msg.pdu.successful_outcome().value.ue_context_setup_resp();
  if (not resp->drbs_setup_list_present) {
    return false;
  }
  return true;
}
