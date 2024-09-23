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
#include "apps/units/flexible_du/split_8/helpers/ru_sdr_logger_registrator.h"
#include "split_8_du_unit_config.h"

namespace srsran {

/// Registers all the loggers for the DU split 8.
inline void register_split_8_du_loggers(const split_8_du_unit_config& config)
{
  register_du_high_loggers(config.du_high_cfg.config.loggers);
  register_du_low_loggers(config.du_low_cfg.loggers);
  register_fapi_loggers(config.fapi_cfg);
  register_ru_sdr_logs(config.ru_cfg.loggers);
}

} // namespace srsran
