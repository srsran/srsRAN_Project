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

namespace srsran {

class task_executor;

/// Interface used to access different executors used in an Open Fronthaul sector.
class ru_ofh_sector_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ru_ofh_sector_executor_mapper() = default;

  /// Retrieves Open Fronthaul sector executor for Ethernet messages transmission and reception.
  virtual task_executor& txrx_executor() = 0;

  /// Retrieves Open Fronthaul sector downlink processing executor.
  virtual task_executor& downlink_executor() = 0;

  /// Retrieves Open Fronthaul sector uplink processing executor.
  virtual task_executor& uplink_executor() = 0;
};

/// \brief Open Fronthaul RU executor mapper interface.
///
/// Provides access to the different sector executor mappers.
class ru_ofh_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ru_ofh_executor_mapper() = default;

  /// \brief Retrieves the Open Fronthaul sector executor mapper for a given sector index.
  ///
  /// \remark An assertion is triggered if the sector index exceeds the number of executor configurations.
  virtual ru_ofh_sector_executor_mapper& get_sector_mapper(unsigned index) = 0;

  /// Retrieves the  Open Fronthaul sector executor mapper via closed braces operator.
  ru_ofh_sector_executor_mapper& operator[](unsigned cell_index) { return get_sector_mapper(cell_index); }

  /// \brief Retrieves Open Fronthaul timing executor.
  ///
  /// \remark this executor is common for all sectors.
  virtual task_executor& timing_executor() = 0;
};

} // namespace srsran
