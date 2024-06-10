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
#include "srsran/cu_cp/cu_cp.h"

namespace srsran {

struct cu_cp_unit_config;
struct worker_manager;

/// CU-CP build dependencies.
struct cu_cp_build_dependencies {
  task_executor*                    cu_cp_executor = nullptr;
  task_executor*                    cu_cp_e2_exec  = nullptr;
  srs_cu_cp::ngap_message_notifier* ngap_notifier  = nullptr;
  timer_manager*                    timers         = nullptr;
};

/// Builds a CU-CP object with the given configuration.
std::unique_ptr<srs_cu_cp::cu_cp> build_cu_cp(const cu_cp_unit_config&  cu_cp_unit_cfg,
                                              cu_cp_build_dependencies& dependencies);

} // namespace srsran
