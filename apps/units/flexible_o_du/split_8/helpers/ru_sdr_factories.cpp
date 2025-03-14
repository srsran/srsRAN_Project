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
#include "srsran/ru/generic/ru_generic_factory.h"

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_sdr_radio_unit(const ru_sdr_unit_config&            ru_cfg,
                                                          const flexible_o_du_ru_config&       ru_config,
                                                          const flexible_o_du_ru_dependencies& ru_dependencies)
{
  ru_generic_configuration config = generate_ru_sdr_config(ru_cfg, ru_config.du_cells, ru_config.max_processing_delay);

  config.rf_logger                   = &srslog::fetch_basic_logger("RF");
  config.radio_exec                  = ru_dependencies.workers.radio_exec;
  config.statistics_printer_executor = ru_dependencies.workers.ru_printer_exec;
  config.timing_notifier             = &ru_dependencies.timing_notifier;
  config.symbol_notifier             = &ru_dependencies.symbol_notifier;
  config.error_notifier              = &ru_dependencies.error_notifier;

  for (unsigned i = 0, e = config.lower_phy_config.size(); i != e; ++i) {
    lower_phy_configuration& low_phy_cfg = config.lower_phy_config[i];
    low_phy_cfg.logger                   = &srslog::fetch_basic_logger("PHY");
    low_phy_cfg.tx_task_executor         = ru_dependencies.workers.lower_phy_tx_exec[i];
    low_phy_cfg.rx_task_executor         = ru_dependencies.workers.lower_phy_rx_exec[i];
    low_phy_cfg.dl_task_executor         = ru_dependencies.workers.lower_phy_dl_exec[i];
    low_phy_cfg.ul_task_executor         = ru_dependencies.workers.lower_phy_ul_exec[i];
    low_phy_cfg.prach_async_executor     = ru_dependencies.workers.lower_prach_exec[i];

    low_phy_cfg.logger->set_level(ru_cfg.loggers.phy_level);
  }

  return create_generic_ru(config);
}
