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

#include "../du_high/logger_registrator.h"
#include "../du_low//logger_registrator.h"
#include "../fapi//logger_registrator.h"

namespace srsran {
namespace modules {
namespace flexible_du {
namespace split_ru_dynamic {

/// Registers all the logs for the DU split RU dynamic.
inline void register_logs(const log_appconfig& log_cfg)
{
  du_high::register_logs(log_cfg);
  du_low::register_logs(log_cfg);
  fapi::register_logs(log_cfg);

  srslog::basic_logger& rf_logger = srslog::fetch_basic_logger("RF", false);
  rf_logger.set_level(srslog::str_to_basic_level(log_cfg.radio_level));

  srslog::basic_logger& ofh_logger = srslog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(srslog::str_to_basic_level(log_cfg.ofh_level));

  srslog::basic_logger& ru_logger = srslog::fetch_basic_logger("RU", true);
  ru_logger.set_level(srslog::str_to_basic_level(log_cfg.radio_level));
}

} // namespace split_ru_dynamic
} // namespace flexible_du
} // namespace modules
} // namespace srsran
