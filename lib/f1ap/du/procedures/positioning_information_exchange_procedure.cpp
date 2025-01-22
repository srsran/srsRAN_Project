/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "positioning_information_exchange_procedure.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<void> srs_du::start_positioning_exchange_procedure(const asn1::f1ap::positioning_info_request_s& msg,
                                                              f1ap_du_configurator&                         du_mng,
                                                              f1ap_du_ue*                                   ue)
{
  srslog::fetch_basic_logger("DU-F1").error(
      "Positioning Exchange Procedure Failed. Cause: Feature only supported in Enterprise version.");
  return launch_async([](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  });
}

#endif // SRSRAN_HAS_ENTERPRISE
