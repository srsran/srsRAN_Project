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

#include "srsran/cu_cp/common_task_scheduler.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/nrppa/nrppa.h"

namespace srsran {
namespace srs_cu_cp {

/// Creates an instance of an NRPPA interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<nrppa_interface> create_nrppa(const cu_cp_configuration& cfg,
                                              nrppa_cu_cp_notifier&      cu_cp_notifier,
                                              common_task_scheduler&     common_task_sched);

} // namespace srs_cu_cp
} // namespace srsran
