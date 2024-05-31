/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
