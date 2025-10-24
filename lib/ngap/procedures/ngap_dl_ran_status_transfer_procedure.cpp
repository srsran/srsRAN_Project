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
