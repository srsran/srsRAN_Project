/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../cu_up_processor/cu_up_processor_repository.h"
#include "../../du_processor/du_processor_repository.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_cu_cp {

async_task<bool> start_inter_cu_handover_source_routine(ue_index_t                  ue_index_,
                                                        byte_buffer                 command,
                                                        ue_manager&                 ue_mng_,
                                                        du_processor_repository&    du_db_,
                                                        cu_up_processor_repository& cu_up_db_,
                                                        srslog::basic_logger&       logger_);

} // namespace srsran::srs_cu_cp
