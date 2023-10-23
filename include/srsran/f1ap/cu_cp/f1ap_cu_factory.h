/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "f1ap_cu.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

/// Creates an instance of an F1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1ap_cu> create_f1ap(f1ap_message_notifier&       f1ap_pdu_notifier_,
                                     f1ap_du_processor_notifier&  f1ap_du_processor_notifier_,
                                     f1ap_du_management_notifier& f1ap_du_management_notifier_,
                                     f1ap_ue_removal_notifier&    f1ap_cu_cp_notifier_,
                                     timer_manager&               timers_,
                                     task_executor&               ctrl_exec_);

} // namespace srs_cu_cp
} // namespace srsran
