/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class f1ap_message_notifier;

namespace srs_du {

class f1ap_du_configurator;

class gnb_cu_configuration_update_procedure
{
public:
  gnb_cu_configuration_update_procedure(const asn1::f1ap::gnb_cu_cfg_upd_s& msg_,
                                        f1ap_du_configurator&               du_mng_,
                                        f1ap_message_notifier&              cu_notif_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  gnbcu_config_update_request request_du_update();

  void send_response();

  const asn1::f1ap::gnb_cu_cfg_upd_s request;
  f1ap_du_configurator&              du_mng;
  f1ap_message_notifier&             cu_notif;

  gnbcu_config_update_response du_resp;
};

} // namespace srs_du
} // namespace srsran
