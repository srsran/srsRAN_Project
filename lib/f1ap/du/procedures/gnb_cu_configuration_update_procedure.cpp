/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_cu_configuration_update_procedure.h"
#include "srsran/f1ap/common/f1ap_common.h"

using namespace srsran;
using namespace srsran::srs_du;

void gnb_cu_configuration_update_procedure::send_gnb_cu_configuration_update_acknowledge()
{
  using namespace asn1::f1ap;

  f1ap_message msg = {};

  msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);
  gnb_cu_cfg_upd_ack_s& ack  = msg.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack();
  ack->transaction_id->value = request->transaction_id->value;

  // send F1AP message.
  cu_notif.on_new_message(msg);
}
