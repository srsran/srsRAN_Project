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

using namespace srsran;

bool srsran::validate_split6_o_du_low_unit_config(const split6_o_du_low_unit_config& config,
                                                  const os_sched_affinity_bitmask&   available_cpus)
{
  // Validate DU low.

  // Validate RU.
  // :TODO: fix the args needed for the validation.
  if (auto* ru = std::get_if<ru_ofh_unit_parsed_config>(&config.ru_cfg)) {
    auto ru_ofh_dependencies = get_ru_ofh_validation_dependencies(config.odu_high_cfg.du_high_cfg.config);
    return validate_ru_ofh_config(ru->config, ru_ofh_dependencies, available_cpus);
  }

  if (auto* ru = std::get_if<ru_sdr_unit_config>(&config.ru_cfg)) {
    if (ru->device_driver == "zmq") {
      fmt::println("Split 6 O-DU low does not support 'zmq' driver");
      return false;
    }

    auto ru_sdr_dependencies = get_ru_sdr_validation_dependencies(config.odu_high_cfg.du_high_cfg.config);
    return validate_ru_sdr_config(*ru, ru_sdr_dependencies, available_cpus);
  }

  return true;
}
