/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ru_ofh_config.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {
struct worker_manager;
class ru_uplink_plane_rx_symbol_notifier;
class ru_timing_notifier;
class ru_error_notifier;

struct du_cell_config;

/// Open Fronthaul RU factory configuration.
struct ru_ofh_factory_config {
  ru_ofh_unit_config         ru_cfg;
  unsigned                   max_processing_delay_slots;
  span<const du_cell_config> du_cells;
};

/// Open Fronthaul RU factory dependencies.
struct ru_ofh_factory_dependencies {
  worker_manager*                     workers         = nullptr;
  ru_uplink_plane_rx_symbol_notifier* symbol_notifier = nullptr;
  ru_timing_notifier*                 timing_notifier = nullptr;
  ru_error_notifier*                  error_notifier  = nullptr;
};

/// Converts and returns the given gnb application configuration to a Radio Unit configuration.
std::unique_ptr<radio_unit> create_ofh_radio_unit(const ru_ofh_factory_config& config,
                                                  ru_ofh_factory_dependencies& dependencies);

} // namespace srsran
