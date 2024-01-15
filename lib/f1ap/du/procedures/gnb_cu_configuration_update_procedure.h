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

#pragma once

#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

class gnb_cu_configuration_update_procedure
{
public:
  explicit gnb_cu_configuration_update_procedure(const asn1::f1ap::gnb_cu_cfg_upd_s& msg_,
                                                 f1ap_message_notifier&              cu_notif_) :
    request(msg_), cu_notif(cu_notif_)
  {
  }

  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);

    send_gnb_cu_configuration_update_acknowledge();

    CORO_RETURN();
  }

private:
  void send_gnb_cu_configuration_update_acknowledge();

  const asn1::f1ap::gnb_cu_cfg_upd_s request;
  f1ap_message_notifier&             cu_notif;
};

} // namespace srs_du
} // namespace srsran
