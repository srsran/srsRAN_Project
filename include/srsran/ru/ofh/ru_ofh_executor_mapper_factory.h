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

#include "srsran/ru/ofh/ru_ofh_executor_mapper.h"
#include <memory>
#include <vector>

namespace srsran {

class task_executor;

/// Configuration of the Open Fronthaul RU executor mapper.
struct ru_ofh_executor_mapper_config {
  unsigned nof_sectors;
  /// Set of executors dedicated for downlink processing in the configured sectors.
  std::vector<task_executor*> downlink_executors;
  /// Set of executors dedicated for uplink processing in the configured sectors.
  std::vector<task_executor*> uplink_executors;
  /// Set of executors dedicated for Ethernet messages reception and transmission in all configured sectors.
  std::vector<task_executor*> txrx_executors;
  /// A single timing executor for all sectors.
  task_executor* timing_executor = nullptr;
};

/// \brief Creates an executor mapper for the Open Fronthaul RU implementation.
///
/// \remark An assertion is triggered if number of downlink and uplink executors doesn't match the number of sectors.
std::unique_ptr<ru_ofh_executor_mapper> create_ofh_ru_executor_mapper(const ru_ofh_executor_mapper_config& config);

} // namespace srsran
