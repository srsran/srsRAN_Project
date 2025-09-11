/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
