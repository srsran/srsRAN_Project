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

#include "ru_ofh_factories.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "ru_ofh_config_translator.h"
#include "srsran/ru/ofh/ru_ofh_factory.h"

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_ofh_radio_unit(const ru_ofh_unit_config&            ru_cfg,
                                                          const flexible_o_du_ru_config&       ru_config,
                                                          const flexible_o_du_ru_dependencies& ru_dependencies)
{
  auto& ofh_exec_map = ru_dependencies.workers.get_ofh_ru_executor_mapper();

  ru_ofh_dependencies dependencies;

  dependencies.logger             = &srslog::fetch_basic_logger("OFH");
  dependencies.rt_timing_executor = &ofh_exec_map.timing_executor();
  dependencies.timing_notifier    = &ru_dependencies.timing_notifier;
  dependencies.rx_symbol_notifier = &ru_dependencies.symbol_notifier;
  dependencies.error_notifier     = &ru_dependencies.error_notifier;

  // Configure sector.
  for (unsigned i = 0, e = ru_cfg.cells.size(); i != e; ++i) {
    ofh::sector_dependencies& sector_deps = dependencies.sector_dependencies.emplace_back();

    // Note, one executor for transmitter and receiver tasks is shared per two sectors.
    sector_deps.txrx_executor     = &ofh_exec_map[i].txrx_executor();
    sector_deps.uplink_executor   = &ofh_exec_map[i].uplink_executor();
    sector_deps.downlink_executor = &ofh_exec_map[i].downlink_executor();
    sector_deps.logger            = dependencies.logger;
  }

  return create_ofh_ru(generate_ru_ofh_config(ru_cfg, ru_config.cells, ru_config.max_processing_delay),
                       std::move(dependencies));
}
