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

#include "split_8_o_du_factory.h"
#include "helpers/ru_sdr_factories.h"
#include "helpers/ru_sdr_helpers.h"

using namespace srsran;

split8_du_factory::split8_du_factory(const split_8_o_du_unit_config& config_) :
  flexible_o_du_factory(
      {config_.odu_high_cfg,
       config_.du_low_cfg,
       {config_.ru_cfg.metrics_cfg.metrics_cfg,
        config_.ru_cfg.metrics_cfg.enable_ru_metrics,
        calculate_dBFS_calibration_value(config_.ru_cfg.srate_MHz,
                                         config_.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell.common_scs)}}),
  unit_config(config_)
{
}

std::unique_ptr<radio_unit> split8_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                                 const flexible_o_du_ru_dependencies& ru_dependencies)
{
  return create_sdr_radio_unit(unit_config.ru_cfg, ru_config, ru_dependencies);
}
