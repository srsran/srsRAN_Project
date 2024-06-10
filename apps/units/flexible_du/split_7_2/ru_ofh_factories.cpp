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

#include "ru_ofh_factories.h"
#include "apps/services/worker_manager.h"
#include "ru_ofh_config_translator.h"
#include "srsran/ru/ru_ofh_factory.h"

using namespace srsran;

// Number of OFH sectors served by a single executor for transmitter and receiver tasks.
static const unsigned NOF_SECTORS_PER_TXRX_EXECUTOR = 2;

/// Resolves the Open Fronthaul Radio Unit dependencies and adds them to the configuration.
static void configure_ru_ofh_executors_and_notifiers(unsigned                            nof_sectors,
                                                     ru_ofh_dependencies&                dependencies,
                                                     worker_manager&                     workers,
                                                     ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                     ru_timing_notifier&                 timing_notifier,
                                                     ru_error_notifier&                  error_notifier)
{
  dependencies.logger             = &srslog::fetch_basic_logger("OFH");
  dependencies.rt_timing_executor = workers.ru_timing_exec;
  dependencies.timing_notifier    = &timing_notifier;
  dependencies.rx_symbol_notifier = &symbol_notifier;
  dependencies.error_notifier     = &error_notifier;

  // Configure sector.
  for (unsigned i = 0; i != nof_sectors; ++i) {
    dependencies.sector_dependencies.emplace_back();
    ru_ofh_sector_dependencies& sector_deps = dependencies.sector_dependencies.back();
    // Note, one executor for transmitter and receiver tasks is shared per two sectors.
    sector_deps.txrx_executor     = workers.ru_txrx_exec[i / NOF_SECTORS_PER_TXRX_EXECUTOR];
    sector_deps.uplink_executor   = workers.ru_rx_exec[i];
    sector_deps.downlink_executor = workers.ru_dl_exec[i];
    sector_deps.logger            = dependencies.logger;
  }
}

std::unique_ptr<radio_unit> srsran::create_ofh_radio_unit(const ru_ofh_factory_config& config,
                                                          ru_ofh_factory_dependencies& dependencies)
{
  srsran_assert(dependencies.error_notifier, "Invalid error notifier");
  srsran_assert(dependencies.symbol_notifier, "Invalid symbol notifier");
  srsran_assert(dependencies.timing_notifier, "Invalid timing notifier");
  srsran_assert(dependencies.workers, "Invalid worker manager");

  ru_ofh_dependencies ru_dependencies;
  configure_ru_ofh_executors_and_notifiers(config.ru_cfg.cells.size(),
                                           ru_dependencies,
                                           *dependencies.workers,
                                           *dependencies.symbol_notifier,
                                           *dependencies.timing_notifier,
                                           *dependencies.error_notifier);

  return create_ofh_ru(generate_ru_ofh_config(config.ru_cfg, config.du_cells, config.max_processing_delay_slots),
                       std::move(ru_dependencies));
}