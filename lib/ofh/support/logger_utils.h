/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

/// Logs the given message as a warning if \c enable_warnings is true, otherwise logs it in info.
template <typename... Args>
void log_conditional_warning(srslog::basic_logger& logger, bool enable_warnings, const char* fmt, Args&&... args)
{
  if (enable_warnings) {
    logger.warning(fmt, std::forward<Args>(args)...);
    return;
  }
  logger.info(fmt, std::forward<Args>(args)...);
}

} // namespace ofh
} // namespace srsran
