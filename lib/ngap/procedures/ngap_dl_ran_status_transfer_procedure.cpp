/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_dl_ran_status_transfer_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<expected<ngap_dl_ran_status_transfer>>
srsran::srs_cu_cp::start_ngap_dl_status_transfer_procedure(ue_index_t                   ue_index,
                                                           ngap_ue_transaction_manager& ev_mng,
                                                           timer_factory                timers,
                                                           ngap_ue_logger&              logger)
{
  logger.log_info("NG Handover failed. Cause: NG handover not supported.");
  auto err_function = [](coro_context<async_task<expected<ngap_dl_ran_status_transfer>>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(make_unexpected(default_error_t{}));
  };
  return launch_async(std::move(err_function));
}

#endif // SRSRAN_HAS_ENTERPRISE
