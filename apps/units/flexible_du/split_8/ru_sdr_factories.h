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

#include "ru_sdr_config.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

namespace srs_du {
struct du_cell_config;
} // namespace srs_du

struct du_high_unit_config;
class ru_uplink_plane_rx_symbol_notifier;
class ru_timing_notifier;
class ru_error_notifier;
struct worker_manager;

/// SDR RU factory configuration.
struct ru_sdr_factory_config {
  ru_sdr_unit_config                 ru_cfg;
  unsigned                           max_processing_delay_slots;
  span<const srs_du::du_cell_config> du_cells;
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
