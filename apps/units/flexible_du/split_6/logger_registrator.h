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
#include "apps/units/flexible_du/du_low//du_low_wrapper_config_helper.h.h"
#include "apps/units/flexible_du/fapi/fapi_logger_registrator.h"

namespace srsran {
namespace modules {
namespace flexible_du {
namespace split_6 {

/// Registers the DU split 6 loggers in the logger service.
inline void register_logs(const log_appconfig& log_cfg)
{
  du_high::register_logs(log_cfg);
  fapi::register_logs(log_cfg);
}

} // namespace split_6
} // namespace flexible_du
} // namespace modules
} // namespace srsran
