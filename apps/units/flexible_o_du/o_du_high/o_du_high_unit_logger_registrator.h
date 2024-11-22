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

#include "du_high/du_high_logger_registrator.h"
#include "fapi/fapi_logger_registrator.h"
#include "o_du_high_unit_config.h"

namespace srsran {

/// Registers the O-RAN DU high loggers in the logger service.
inline void register_o_du_high_loggers(const o_du_high_unit_config& unit_config)
{
  register_du_high_loggers(unit_config.du_high_cfg.config.loggers);
  register_fapi_loggers(unit_config.fapi_cfg);
}

} // namespace srsran
