/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ngap.h"
#include "ngap_configuration.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {

namespace srs_cu_cp {

/// Creates an instance of an NGAP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<ngap_interface> create_ngap(ngap_configuration&                ngap_cfg_,
                                            ngap_cu_cp_du_repository_notifier& cu_cp_du_repository_notifier_,
                                            ngap_ue_task_scheduler&            task_sched_,
                                            ngap_ue_manager&                   ue_manager_,
                                            ngap_message_notifier&             ngap_notifier_,
                                            task_executor&                     ctrl_exec_);

} // namespace srs_cu_cp

} // namespace srsran
