/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_common_messages.h"
#include "srsran/asn1/e1ap/common.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"

using namespace srsran;
using namespace asn1::e1ap;

e1ap_message srsran::generate_error_indication(uint8_t                                      transaction_id,
                                               const std::optional<gnb_cu_up_ue_e1ap_id_t>& cu_up_ue_id,
                                               const std::optional<gnb_cu_cp_ue_e1ap_id_t>& cu_cp_ue_id,
                                               const std::optional<asn1::e1ap::cause_c>&    cause)
{
  e1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_E1AP_ID_ERROR_IND);
  asn1::e1ap::error_ind_s& error_ind = msg.pdu.init_msg().value.error_ind();

  error_ind->transaction_id = transaction_id;

  error_ind->gnb_cu_up_ue_e1ap_id_present = cu_up_ue_id.has_value();
  if (cu_up_ue_id.has_value()) {
    error_ind->gnb_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_to_uint(*cu_up_ue_id);
  }

  error_ind->gnb_cu_cp_ue_e1ap_id_present = cu_cp_ue_id.has_value();
  if (cu_cp_ue_id.has_value()) {
    error_ind->gnb_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_to_uint(*cu_cp_ue_id);
  }

  error_ind->cause_present = cause.has_value();
  if (error_ind->cause_present) {
    error_ind->cause = *cause;
  }

  return msg;
}
