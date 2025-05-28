/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
