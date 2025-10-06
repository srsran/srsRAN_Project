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

#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/ngap/ngap_handover.h"

namespace srsran::srs_cu_cp {

async_task<void> start_inter_cu_handover_execution_target_routine(cu_cp_ue*                    ue,
                                                                  e1ap_bearer_context_manager& e1ap,
                                                                  ngap_interface&              ngap,
                                                                  srslog::basic_logger&        logger);

} // namespace srsran::srs_cu_cp
