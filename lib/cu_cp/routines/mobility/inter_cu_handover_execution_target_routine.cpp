/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "inter_cu_handover_execution_target_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<void>
srsran::srs_cu_cp::start_inter_cu_handover_execution_target_routine(ue_index_t                   ue_index,
                                                                    const up_resource_manager&   up_manager,
                                                                    e1ap_bearer_context_manager& e1ap,
                                                                    ngap_interface&              ngap,
                                                                    srslog::basic_logger&        logger)
{
  logger.error("Inter-CU handover execution target routine failed. Cause: Inter-CU handover not supported");

  auto err_function = [](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  };
  return launch_async(std::move(err_function));
}

#endif // SRSRAN_HAS_ENTERPRISE
