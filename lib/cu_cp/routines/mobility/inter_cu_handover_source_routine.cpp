/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "inter_cu_handover_source_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<bool> srsran::srs_cu_cp::start_inter_cu_handover_source_routine(ue_index_t                  ue_index,
                                                                           byte_buffer                 command,
                                                                           ue_manager&                 ue_mng,
                                                                           du_processor_repository&    du_db,
                                                                           cu_up_processor_repository& cu_up_db,
                                                                           srslog::basic_logger&       logger)
{
  logger.error("Inter-CU handover source routine failed. Cause: Inter-CU handover not supported");

  auto err_function = [](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(false);
  };
  return launch_async(std::move(err_function));
}
#endif // SRSRAN_HAS_ENTERPRISE
