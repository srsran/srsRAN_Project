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
#include "apps/units/flexible_du/split_7_2/helpers/ru_ofh_logger_registrator.h"
#include "split_7_2_du_unit_config.h"

namespace srsran {

/// Registers all the loggers for the DU split 7.2.
inline void register_split_7_2_du_loggers(const split_7_2_du_unit_config& config)
{
  register_du_high_loggers(config.du_high_cfg.config.loggers);
  register_du_low_loggers(config.du_low_cfg.loggers);
  register_fapi_loggers(config.fapi_cfg);
  register_ru_ofh_loggers(config.ru_cfg.config.loggers);
}

} // namespace srsran
