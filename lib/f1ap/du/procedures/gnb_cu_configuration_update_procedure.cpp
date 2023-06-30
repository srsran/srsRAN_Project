/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "gnb_cu_configuration_update_procedure.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;
using namespace srsran::srs_du;

void gnb_cu_configuration_update_procedure::send_gnb_cu_configuration_update_acknowledge()
{
  using namespace asn1::f1ap;

  f1ap_message msg = {};

  msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_GNB_CU_CFG_UPD);
  gnb_cu_cfg_upd_ack_s& ack = msg.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack();
  ack->transaction_id       = request->transaction_id;

  // send F1AP message.
  cu_notif.on_new_message(msg);
}
