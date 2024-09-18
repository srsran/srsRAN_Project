/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_factories.h"
#include "apps/services/worker_manager.h"
#include "ru_ofh_config_translator.h"
#include "srsran/ru/ru_ofh_factory.h"

using namespace srsran;

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

  // Number of OFH sectors served by a single executor for transmitter and receiver tasks.
  unsigned nof_txrx_threads = workers.ru_txrx_exec.size();
  unsigned nof_sectors_per_txrx_thread =
      (nof_sectors > nof_txrx_threads) ? static_cast<unsigned>(std::ceil(nof_sectors / float(nof_txrx_threads))) : 1;

  // Configure sector.
  for (unsigned i = 0; i != nof_sectors; ++i) {
    dependencies.sector_dependencies.emplace_back();
    ru_ofh_sector_dependencies& sector_deps = dependencies.sector_dependencies.back();
    // Note, one executor for transmitter and receiver tasks is shared per two sectors.
    sector_deps.txrx_executor     = workers.ru_txrx_exec[i / nof_sectors_per_txrx_thread];
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