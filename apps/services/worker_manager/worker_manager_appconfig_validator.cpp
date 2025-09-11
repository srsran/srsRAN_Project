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

#include "worker_manager_appconfig_validator.h"
#include "worker_manager_appconfig.h"

using namespace srsran;

bool srsran::validate_expert_execution_appconfig(const expert_execution_appconfig& config)
{
  // Ensure the number of non-real time threads does not exceed the number of CPU cores
  auto&    cpu_desc  = cpu_architecture_info::get();
  uint32_t nof_cores = cpu_desc.get_host_nof_available_cpus();
  if (config.threads.main_pool.nof_threads and config.threads.main_pool.nof_threads.value() > nof_cores) {
    fmt::print("Invalid expert execution config: main_pool.nof_threads={} must not exceed nof_cores={}\n",
               config.threads.main_pool.nof_threads.value(),
               nof_cores);
    return false;
  }

  return true;
}
