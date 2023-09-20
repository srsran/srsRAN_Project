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

#include "e1ap_cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_connection_client.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <memory>

namespace srsran {
namespace srs_cu_up {

/// Creates an instance of an E1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1ap_interface> create_e1ap(e1ap_connection_client& e1ap_client_handler_,
                                            e1ap_cu_up_notifier&    cu_up_notifier_,
                                            timer_manager&          timers_,
                                            task_executor&          cu_up_exec_);

} // namespace srs_cu_up
} // namespace srsran
