/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_logger_registrator.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_logger_registrator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_logger_registrator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_logger_registrator.h"
#include "dynamic_o_du_unit_config.h"

namespace srsran {

/// Registers all the loggers for the O-RAN DU split dynamic.
inline void register_dynamic_o_du_loggers(const dynamic_o_du_unit_config& config)
{
  register_o_du_high_loggers(config.odu_high_cfg);
  register_du_low_loggers(config.du_low_cfg.loggers);

  if (std::holds_alternative<ru_sdr_unit_config>(config.ru_cfg)) {
    register_ru_sdr_logs(std::get<ru_sdr_unit_config>(config.ru_cfg).loggers);
  }

  if (std::holds_alternative<ru_ofh_unit_parsed_config>(config.ru_cfg)) {
    register_ru_ofh_loggers(std::get<ru_ofh_unit_parsed_config>(config.ru_cfg).config.loggers);
  }
}

} // namespace srsran
