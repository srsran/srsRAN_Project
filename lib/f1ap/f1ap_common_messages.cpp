/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_common_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"

using namespace srsran;
using namespace asn1::f1ap;

f1ap_message srsran::generate_error_indication(uint8_t                                   transaction_id,
                                               const std::optional<gnb_du_ue_f1ap_id_t>& du_ue_id,
                                               const std::optional<gnb_cu_ue_f1ap_id_t>& cu_ue_id,
                                               const std::optional<asn1::f1ap::cause_c>& cause)
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_ERROR_IND);
  asn1::f1ap::error_ind_s error_ind = msg.pdu.init_msg().value.error_ind();

  error_ind->transaction_id = transaction_id;

  error_ind->gnb_du_ue_f1ap_id_present = du_ue_id.has_value();
  if (du_ue_id.has_value()) {
    error_ind->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(*du_ue_id);
  }

  error_ind->gnb_cu_ue_f1ap_id_present = cu_ue_id.has_value();
  if (cu_ue_id.has_value()) {
    error_ind->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(*cu_ue_id);
  }

  error_ind->cause_present = cause.has_value();
  if (error_ind->cause_present) {
    error_ind->cause = *cause;
  }

  return msg;
}
