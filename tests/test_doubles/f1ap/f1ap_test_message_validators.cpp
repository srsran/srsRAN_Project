/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_test_message_validators.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;

bool srsran::test_helpers::is_init_ul_rrc_msg_transfer_valid(const f1ap_message& msg, rnti_t rnti)
{
  if (not(msg.pdu.type() == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg and
          msg.pdu.init_msg().proc_code == ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER)) {
    return false;
  }
  const asn1::f1ap::init_ul_rrc_msg_transfer_s& rrcmsg = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
  return rrcmsg->c_rnti == rnti;
}
