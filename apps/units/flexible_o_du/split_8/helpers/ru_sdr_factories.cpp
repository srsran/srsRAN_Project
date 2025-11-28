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

#include "ru_sdr_factories.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "ru_sdr_config_translator.h"
#include "srsran/ru/sdr/ru_sdr_factory.h"

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_sdr_radio_unit(const ru_sdr_unit_config&            ru_sdr_cfg,
                                                          const flexible_o_du_ru_config&       du_ru_config,
                                                          const flexible_o_du_ru_dependencies& ru_dependencies)
{
  ru_sdr_configuration config =
      generate_ru_sdr_config(ru_sdr_cfg, du_ru_config.cells, du_ru_config.max_processing_delay);

  ru_sdr_executor_mapper& exec_map = ru_dependencies.workers.get_sdr_ru_executor_mapper();

  ru_sdr_dependencies deps = {
      .radio_exec      = exec_map.asynchronous_radio_executor(),
      .rf_logger       = srslog::fetch_basic_logger("RF"),
      .symbol_notifier = ru_dependencies.symbol_notifier,
      .timing_notifier = ru_dependencies.timing_notifier,
      .error_notifier  = ru_dependencies.error_notifier,
  };

  deps.sector_dependencies.reserve(config.lower_phy_config.size());
  for (unsigned i = 0, e = config.lower_phy_config.size(); i != e; ++i) {
    ru_sdr_sector_executor_mapper& sector_exec_map = exec_map[i];

    deps.sector_dependencies.emplace_back(
        ru_sdr_sector_dependencies{.logger               = srslog::fetch_basic_logger("PHY"),
                                   .rx_task_executor     = sector_exec_map.receiver_executor(),
                                   .tx_task_executor     = sector_exec_map.transmitter_executor(),
                                   .dl_task_executor     = sector_exec_map.downlink_executor(),
                                   .ul_task_executor     = sector_exec_map.uplink_executor(),
                                   .prach_async_executor = sector_exec_map.prach_executor()});
  }

  return create_sdr_ru(config, deps);
}
