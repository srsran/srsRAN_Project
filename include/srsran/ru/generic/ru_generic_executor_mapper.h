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

#include "srsran/support/executors/task_executor.h"
#include <memory>
#include <vector>

namespace srsran {

class task_executor;

/// Interface used to access different executors used in an generic radio unit sector.
class ru_sdr_sector_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ru_sdr_sector_executor_mapper() = default;

  /// Retrieves lower physical layer executor for downlink modulation.
  virtual task_executor& downlink_executor() = 0;

  /// Retrieves lower physical layer executor for uplink demodulation.
  virtual task_executor& uplink_executor() = 0;

  /// Retrieves lower physical layer executor for PRACH demodulation.
  virtual task_executor& prach_executor() = 0;

  /// Retrieves lower physical layer executor for baseband transmission.
  virtual task_executor& transmitter_executor() = 0;

  /// Retrieves lower physical layer executor for baseband reception.
  virtual task_executor& receiver_executor() = 0;
};

/// \brief Generic RU executor mapper interface.
///
/// Provides access to the different sector executor mappers.
class ru_generic_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ru_generic_executor_mapper() = default;

  /// \brief Retrieves the generic sector executor mapper for a given sector index.
  ///
  /// \remark An assertion is triggered if the sector index exceeds the number of executor configurations.
  virtual ru_sdr_sector_executor_mapper& get_sector_mapper(unsigned index) = 0;

  /// Retrieves the generic sector executor mapper via closed braces operator.
  ru_sdr_sector_executor_mapper& operator[](unsigned cell_index) { return get_sector_mapper(cell_index); }

  /// Retrieves baseband radio house-keeping executor.
  virtual task_executor& asynchronous_radio_executor() = 0;
};

/// Collects the generic RU executor mapper executors per sector.
struct ru_generic_executor_mapper_sector_configuration {
  /// Downlink modulator executor.
  task_executor* dl_exec;
  /// Uplink modulator executor.
  task_executor* ul_exec;
  /// PRACH modulator executor.
  task_executor* prach_exec;
  /// Baseband transmitter executor.
  task_executor* tx_exec;
  /// Baseband receiver executor.
  task_executor* rx_exec;
};

/// Collects the generic RU executor mapper dependencies.
struct ru_generic_executor_mapper_configuration {
  /// Common radio executor.
  task_executor* radio_exec;
  /// Sector executors.
  std::vector<ru_generic_executor_mapper_sector_configuration> sectors;
};

/// Creates a generic RU executor mapper.
std::unique_ptr<ru_generic_executor_mapper>
create_ru_generic_executor_mapper(ru_generic_executor_mapper_configuration& config);

} // namespace srsran
