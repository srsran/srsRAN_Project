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

#include "srsran/adt/optional.h"
#include "srsran/phy/lower/lower_phy_configuration.h"
#include "srsran/radio/radio_configuration.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// Radio Unit generic configuration.
struct ru_generic_configuration {
  /// Maximum number of PRACH concurrent requests.
  unsigned max_nof_prach_concurrent_requests = 11;
  /// Device driver.
  std::string device_driver;
  /// Radio configuration.
  radio_configuration::radio radio_cfg;
  /// Lower PHY configurations.
  std::vector<lower_phy_configuration> lower_phy_config;
  /// Radio executor.
  task_executor* radio_exec = nullptr;
  /// Radio logger.
  srslog::basic_logger* rf_logger = nullptr;
  /// Radio Unit uplink plane received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier* symbol_notifier = nullptr;
  /// Radio Unit timing notifier.
  ru_timing_notifier* timing_notifier = nullptr;
  /// Statistics printer executor.
  task_executor* statistics_printer_executor = nullptr;
  /// \brief Statistics print interval in seconds.
  ///
  /// The statistics of the generic Radio Unit implementation will be printed every statistics_print_interval_s seconds.
  unsigned statistics_print_interval_s = 1;
};

} // namespace srsran
