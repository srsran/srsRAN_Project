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

#include "ru_sdr_config.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

struct du_cell_config;
struct du_high_unit_config;
class ru_uplink_plane_rx_symbol_notifier;
class ru_timing_notifier;
class ru_error_notifier;
struct worker_manager;

/// SDR RU factory configuration.
struct ru_sdr_factory_config {
  ru_sdr_unit_config         ru_cfg;
  unsigned                   max_processing_delay_slots;
  span<const du_cell_config> du_cells;
};

/// SDR RU factory dependencies.
struct ru_sdr_factory_dependencies {
  worker_manager*                     workers         = nullptr;
  ru_uplink_plane_rx_symbol_notifier* symbol_notifier = nullptr;
  ru_timing_notifier*                 timing_notifier = nullptr;
  ru_error_notifier*                  error_notifier  = nullptr;
};

/// Creates and returns a SDR RU with the given consifuration and dependencies.
std::unique_ptr<radio_unit> create_sdr_radio_unit(const ru_sdr_factory_config& config,
                                                  ru_sdr_factory_dependencies& dependencies);

} // namespace srsran
