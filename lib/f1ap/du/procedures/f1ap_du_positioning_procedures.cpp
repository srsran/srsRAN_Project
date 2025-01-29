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

#ifndef SRSRAN_HAS_ENTERPRISE

#include "f1ap_du_positioning_procedures.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

async_task<void> srs_du::start_positioning_measurement_procedure(const asn1::f1ap::positioning_meas_request_s& msg,
                                                                 f1ap_du_configurator&                         du_mng,
                                                                 f1ap_message_notifier& cu_notifier)
{
  return launch_async([](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    srslog::fetch_basic_logger("DU-F1").error(
        "Positioning Measurement Procedure Failed. Cause: Feature only supported in Enterprise version.");
    CORO_RETURN();
  });
}

async_task<void> srs_du::start_trp_information_exchange_procedure(const asn1::f1ap::trp_info_request_s& msg,
                                                                  f1ap_du_configurator&                 du_mng,
                                                                  f1ap_message_notifier&                cu_notifier)
{
  return launch_async([](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    srslog::fetch_basic_logger("DU-F1").error(
        "TRP Information Exchange Procedure Failed. Cause: Feature only supported in Enterprise version.");
    CORO_RETURN();
  });
}

async_task<void> srs_du::start_positioning_exchange_procedure(const asn1::f1ap::positioning_info_request_s& msg,
                                                              f1ap_du_configurator&                         du_mng,
                                                              f1ap_du_ue&                                   ue)
{
  return launch_async([](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    srslog::fetch_basic_logger("DU-F1").error(
        "Positioning Exchange Procedure Failed. Cause: Feature only supported in Enterprise version.");
    CORO_RETURN();
  });
}

#endif // SRSRAN_HAS_ENTERPRISE
