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

#include "srsran/e1ap/cu_cp/e1ap_configuration.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

/// Creates an instance of an E1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1ap_interface> create_e1ap(const e1ap_configuration&      e1ap_cfg_,
                                            cu_up_index_t                  cu_up_index_,
                                            e1ap_message_notifier&         e1ap_pdu_notifier_,
                                            e1ap_cu_up_processor_notifier& e1ap_cu_up_processor_notifier_,
                                            e1ap_cu_cp_notifier&           cu_cp_notifier_,
                                            timer_manager&                 timers_,
                                            task_executor&                 ctrl_exec_,
                                            unsigned                       max_nof_supported_ues_);

} // namespace srs_cu_cp
} // namespace srsran
