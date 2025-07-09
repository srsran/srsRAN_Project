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
