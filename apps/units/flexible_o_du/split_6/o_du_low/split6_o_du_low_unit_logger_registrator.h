/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  register_du_low_loggers(config.du_low_cfg.loggers);

  if (auto* ru_cfg = std::get_if<ru_sdr_unit_config>(&config.ru_cfg)) {
    register_ru_sdr_logs(ru_cfg->loggers);
  } else if (auto* ru_cfg = std::get_if<ru_ofh_unit_parsed_config>(&config.ru_cfg)) {
    register_ru_ofh_loggers(ru_cfg->config.loggers);
  }
}

} // namespace srsran
