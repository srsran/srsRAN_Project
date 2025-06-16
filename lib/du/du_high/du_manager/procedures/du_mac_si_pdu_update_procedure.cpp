/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
