/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/f1ap/common/f1ap_common.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
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
} // namespace srsgnb
