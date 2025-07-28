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

#include "split6_o_du_low_unit_config_validator.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_validator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_validator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_validator.h"

using namespace srsran;

bool srsran::validate_split6_o_du_low_unit_config(const split6_o_du_low_unit_config& config,
                                                  const os_sched_affinity_bitmask&   available_cpus)
{
  validate_du_low_cpus(config.du_low_cfg, available_cpus);

  // Validate RU.
  if (auto* ru = std::get_if<ru_ofh_unit_parsed_config>(&config.ru_cfg)) {
    return validate_ru_ofh_cpus(ru->config, available_cpus);
  }

  if (auto* ru = std::get_if<ru_sdr_unit_config>(&config.ru_cfg)) {
    if (ru->device_driver == "zmq") {
      fmt::println("Split 6 O-DU low does not support 'zmq' driver");
      return false;
    }

    return validate_ru_sdr_cpus(*ru, available_cpus);
  }

  return true;
}
