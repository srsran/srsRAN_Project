/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_removal_procedure.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

using namespace srsran;
using namespace srs_cu_cp;

void srsran::srs_cu_cp::handle_f1_removal_procedure(const asn1::f1ap::f1_removal_request_s& request,
                                                    du_index_t                              du_index,
                                                    f1ap_message_notifier&                  pdu_notifier,
                                                    f1ap_du_management_notifier&            du_management_notifier,
                                                    srslog::basic_logger&                   logger)
{
  // Send F1 Removal Response
  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  asn1::f1ap::f1_removal_resp_s& rem = resp.pdu.successful_outcome().value.f1_removal_resp();
  rem->transaction_id                = (*request)[0]->transaction_id();
  pdu_notifier.on_new_message(resp);
}
