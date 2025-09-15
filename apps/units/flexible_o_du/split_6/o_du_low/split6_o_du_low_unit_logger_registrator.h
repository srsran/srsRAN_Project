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

#pragma once

#include "apps/units/flexible_o_du/o_du_low/du_low_logger_registrator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_logger_registrator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_logger_registrator.h"
#include "split6_o_du_low_unit_config.h"
#include <variant>

namespace srsran {

/// Registers all the loggers for the O-RAN DU low split 6.
inline void register_split6_o_du_low_loggers(const split6_o_du_low_unit_config& config)
{
  auto& fapi = srslog::fetch_basic_logger("FAPI", true);
  fapi.set_level(config.fapi_level);

  register_du_low_loggers(config.du_low_cfg.loggers);

  if (const auto* ru_sdr = std::get_if<ru_sdr_unit_config>(&config.ru_cfg)) {
    register_ru_sdr_logs(ru_sdr->loggers);
  } else if (const auto* ru_ofh = std::get_if<ru_ofh_unit_parsed_config>(&config.ru_cfg)) {
    register_ru_ofh_loggers(ru_ofh->config.loggers);
  }
}

} // namespace srsran
