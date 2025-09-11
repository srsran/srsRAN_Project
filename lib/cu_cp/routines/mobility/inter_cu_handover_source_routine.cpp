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

#include "inter_cu_handover_source_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<bool> srsran::srs_cu_cp::start_inter_cu_handover_source_routine(ue_index_t                    ue_index,
                                                                           byte_buffer                   command,
                                                                           ue_manager&                   ue_mng,
                                                                           du_processor_repository&      du_db,
                                                                           cu_up_processor_repository&   cu_up_db,
                                                                           ngap_control_message_handler& ngap,
                                                                           srslog::basic_logger&         logger)
{
  logger.error("Inter-CU handover source routine failed. Cause: Inter-CU handover not supported");

  auto err_function = [](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(false);
  };
  return launch_async(std::move(err_function));
}
#endif // SRSRAN_HAS_ENTERPRISE
