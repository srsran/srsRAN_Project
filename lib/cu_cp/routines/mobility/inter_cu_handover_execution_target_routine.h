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

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/ngap/ngap_handover.h"

namespace srsran::srs_cu_cp {

async_task<void> start_inter_cu_handover_execution_target_routine(cu_cp_ue*                    ue,
                                                                  e1ap_bearer_context_manager& e1ap,
                                                                  ngap_interface&              ngap,
                                                                  srslog::basic_logger&        logger);

} // namespace srsran::srs_cu_cp
