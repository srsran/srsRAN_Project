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

#include "ru_ofh_config.h"

namespace srsran {

/// Registers the Open Fronthaul Radio Unit loggers in the logger service.
inline void register_ru_ofh_loggers(const ru_ofh_unit_logger_config& log_cfg)
{
  srslog::basic_logger& ofh_logger = srslog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(srslog::str_to_basic_level(log_cfg.ofh_level));
}

} // namespace srsran
