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

#include "ru_sdr_config.h"

namespace srsran {

/// Registers the SDR Radio Unit loggers in the logger service.
inline void register_ru_sdr_logs(const ru_sdr_unit_logger_config& log_cfg)
{
  srslog::basic_logger& rf_logger = srslog::fetch_basic_logger("RF", false);
  rf_logger.set_level(log_cfg.radio_level);

  srslog::basic_logger& ru_logger = srslog::fetch_basic_logger("RU", true);
  ru_logger.set_level(log_cfg.radio_level);
}

} // namespace srsran
