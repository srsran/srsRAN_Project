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

#include "dynamic_o_du_factory.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_factories.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_factories.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_helpers.h"
#include "dynamic_o_du_translators.h"
#include "srsran/ru/dummy/ru_dummy_factory.h"

using namespace srsran;

static flexible_o_du_unit_config::ru_config
generate_ru_config(const std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config, ru_dummy_unit_config>& ru_cfg,
                   subcarrier_spacing                                                                       scs)
{
  if (const auto* ru = std::get_if<ru_sdr_unit_config>(&ru_cfg)) {
    return {ru->metrics_cfg.metrics_cfg,
            ru->metrics_cfg.enable_ru_metrics,
            calculate_dBFS_calibration_value(ru->srate_MHz, scs)};
  }

  if (const auto* ru = std::get_if<ru_dummy_unit_config>(&ru_cfg)) {
    return {ru->metrics_cfg.metrics_cfg, ru->metrics_cfg.enable_ru_metrics};
  }

  if (const auto* ru = std::get_if<ru_ofh_unit_parsed_config>(&ru_cfg)) {
    return {ru->config.metrics_cfg.metrics_cfg, ru->config.metrics_cfg.enable_ru_metrics};
  }

  return {};
}

dynamic_o_du_factory::dynamic_o_du_factory(const dynamic_o_du_unit_config& config_) :
  flexible_o_du_factory(
      {config_.odu_high_cfg,
       config_.du_low_cfg,
       generate_ru_config(config_.ru_cfg, config_.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell.common_scs)}),
  unit_config(config_)
{
}

static std::unique_ptr<radio_unit> create_dummy_radio_unit(const ru_dummy_unit_config&          ru_cfg,
                                                           const flexible_o_du_ru_config&       ru_config,
                                                           const flexible_o_du_ru_dependencies& ru_dependencies)
{
  ru_dummy_dependencies dependencies{
      .logger          = srslog::fetch_basic_logger("RU", true),
      .executor        = &ru_dependencies.workers.get_dummy_ru_executor_mapper().common_executor(),
      .symbol_notifier = ru_dependencies.symbol_notifier,
      .timing_notifier = ru_dependencies.timing_notifier,
      .error_notifier  = ru_dependencies.error_notifier,
  };

  return create_dummy_ru(
      generate_ru_dummy_config(ru_cfg, ru_config.cells, ru_config.max_processing_delay, ru_config.prach_nof_ports),
      dependencies);
}

std::unique_ptr<radio_unit>
dynamic_o_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                        const flexible_o_du_ru_dependencies& ru_dependencies)
{
  const auto& ru_cfg = unit_config.ru_cfg;
  if (const auto* cfg = std::get_if<ru_ofh_unit_parsed_config>(&ru_cfg)) {
    return create_ofh_radio_unit(cfg->config, ru_config, ru_dependencies);
  }

  if (const auto* cfg = std::get_if<ru_sdr_unit_config>(&ru_cfg)) {
    return create_sdr_radio_unit(*cfg, ru_config, ru_dependencies);
  }

  return create_dummy_radio_unit(std::get<ru_dummy_unit_config>(ru_cfg), ru_config, ru_dependencies);
}
