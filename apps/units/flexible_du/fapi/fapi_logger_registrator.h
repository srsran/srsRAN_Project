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

#include "fapi_config.h"

namespace srsran {

/// Registers the fapi loggers in the logger service.
inline void register_fapi_loggers(const fapi_unit_config& log_cfg)
{
  auto& fapi_logger = srslog::fetch_basic_logger("FAPI", true);
  fapi_logger.set_level(srslog::str_to_basic_level(log_cfg.fapi_level));
}

} // namespace srsran
