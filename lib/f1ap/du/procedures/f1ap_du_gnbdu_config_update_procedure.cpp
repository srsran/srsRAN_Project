/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_gnbdu_config_update_procedure.h"

using namespace srsran;
using namespace srs_du;

f1ap_du_gnbdu_config_update_procedure::f1ap_du_gnbdu_config_update_procedure(
    const gnbdu_config_update_request& request_,
    f1ap_message_notifier&             cu_notif_) :
  request(request_), cu_notif(cu_notif_)
{
}

void f1ap_du_gnbdu_config_update_procedure::operator()(coro_context<async_task<gnbdu_config_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  CORO_RETURN(resp);
}
