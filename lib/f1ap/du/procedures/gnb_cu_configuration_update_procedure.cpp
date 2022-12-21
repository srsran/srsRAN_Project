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
#include "srsgnb/f1ap/common/f1ap_common.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

void gnb_cu_configuration_update_procedure::send_gnb_cu_configuration_update_acknowledge()
{
  using namespace asn1::f1ap;

  f1c_message msg = {};

  msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_G_NBCU_CFG_UPD);
  gnbcu_cfg_upd_ack_s& ack   = msg.pdu.successful_outcome().value.gnbcu_cfg_upd_ack();
  ack->transaction_id->value = request->transaction_id->value;

  // send F1c message.
  cu_notif.on_new_message(msg);
}
