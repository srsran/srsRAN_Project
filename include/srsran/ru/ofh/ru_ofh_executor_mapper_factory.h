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
  /// Number of sectors.
  unsigned nof_sectors;
  /// Executor dedicated to IQ sample compression and OFH message serialization.
  task_executor* downlink_executor;
  /// Executor dedicated to IQ sample decompression and OFH message deserialization.
  task_executor* uplink_executor;
  /// Set of executors dedicated to Ethernet messages reception and transmission for all configured sectors.
  std::vector<task_executor*> txrx_executors;
  /// A single timing executor for all sectors.
  task_executor* timing_executor = nullptr;
};

/// \brief Creates an executor mapper for the Open Fronthaul RU implementation.
///
/// \remark An assertion is triggered if number of downlink and uplink executors doesn't match the number of sectors.
std::unique_ptr<ru_ofh_executor_mapper> create_ofh_ru_executor_mapper(const ru_ofh_executor_mapper_config& config);

} // namespace srsran
