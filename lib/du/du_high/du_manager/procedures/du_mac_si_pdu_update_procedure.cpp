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

#include "du_mac_si_pdu_update_procedure.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_no_op_task.h"
#include <future>

using namespace srsran;
using namespace srs_du;

#ifndef SRSRAN_HAS_ENTERPRISE_NTN

async_task<du_si_pdu_update_response> srsran::srs_du::start_du_mac_si_pdu_update(const du_si_pdu_update_request& req,
                                                                                 const du_manager_params&        params,
                                                                                 du_cell_manager& cell_mng)
{
  auto err_function = [](coro_context<async_task<du_si_pdu_update_response>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(du_si_pdu_update_response{false});
  };
  return launch_async(std::move(err_function));
}

#endif // SRSRAN_HAS_ENTERPRISE_NTN
