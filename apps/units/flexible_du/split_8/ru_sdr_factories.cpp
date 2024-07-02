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

#include "ru_sdr_factories.h"
#include "apps/services/worker_manager.h"
#include "ru_sdr_config_translator.h"
#include "srsran/ru/ru_generic_factory.h"

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_sdr_radio_unit(const ru_sdr_factory_config& config,
                                                          ru_sdr_factory_dependencies& dependencies)
{
  ru_generic_configuration ru_config =
      generate_ru_sdr_config(config.ru_cfg, config.du_cells, config.max_processing_delay_slots);

  ru_config.rf_logger                   = &srslog::fetch_basic_logger("RF");
  ru_config.radio_exec                  = dependencies.workers->radio_exec;
  ru_config.statistics_printer_executor = dependencies.workers->ru_printer_exec;
  ru_config.timing_notifier             = dependencies.timing_notifier;
  ru_config.symbol_notifier             = dependencies.symbol_notifier;

  for (unsigned i = 0, e = ru_config.lower_phy_config.size(); i != e; ++i) {
    lower_phy_configuration& low_phy_cfg = ru_config.lower_phy_config[i];
    low_phy_cfg.logger                   = &srslog::fetch_basic_logger("PHY");
    low_phy_cfg.tx_task_executor         = dependencies.workers->lower_phy_tx_exec[i];
    low_phy_cfg.rx_task_executor         = dependencies.workers->lower_phy_rx_exec[i];
    low_phy_cfg.dl_task_executor         = dependencies.workers->lower_phy_dl_exec[i];
    low_phy_cfg.ul_task_executor         = dependencies.workers->lower_phy_ul_exec[i];
    low_phy_cfg.prach_async_executor     = dependencies.workers->lower_prach_exec[i];

    low_phy_cfg.logger->set_level(config.ru_cfg.loggers.phy_level);
  }

  return create_generic_ru(ru_config);
}