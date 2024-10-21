/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_dl_ue_associated_nrppa_transport_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

SRSRAN_WEAK_SYMB async_task<void>
srsran::srs_cu_cp::start_ngap_dl_ue_associated_nrppa_transport(const ngap_ue_associated_nrppa_transport& msg,
                                                               ngap_cu_cp_notifier&                      cu_cp_notifier,
                                                               ngap_message_notifier&                    amf_notifier,
                                                               ngap_ue_logger&                           logger)
{
  logger.log_error("DL UE associated NRPPa transport failed. Cause: NRPPa transport messages not supported.");
  auto err_function = [](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  };
  return launch_async(std::move(err_function));
}
