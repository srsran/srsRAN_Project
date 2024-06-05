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

#include "apps/units/flexible_du/du_high/du_high_logger_registrator.h"
#include "apps/units/flexible_du/du_low/du_low_logger_registrator.h"
#include "apps/units/flexible_du/fapi/fapi_logger_registrator.h"
#include "apps/units/flexible_du/split_7_2/ru_ofh_logger_registrator.h"
#include "apps/units/flexible_du/split_8/ru_sdr_logger_registrator.h"
#include "dynamic_du_unit_config.h"

namespace srsran {

/// Registers all the loggers for the DU split dynamic.
inline void register_dynamic_du_loggers(const dynamic_du_unit_config& config)
{
  register_du_high_loggers(config.du_high_cfg.config.loggers);
  register_du_low_loggers(config.du_low_cfg.loggers);
  register_fapi_loggers(config.fapi_cfg);

  if (std::holds_alternative<ru_sdr_unit_config>(config.ru_cfg)) {
    register_ru_sdr_logs(std::get<ru_sdr_unit_config>(config.ru_cfg).loggers);
  }

  if (std::holds_alternative<ru_ofh_unit_parsed_config>(config.ru_cfg)) {
    register_ru_ofh_loggers(std::get<ru_ofh_unit_parsed_config>(config.ru_cfg).config.loggers);
  }
}

} // namespace srsran
