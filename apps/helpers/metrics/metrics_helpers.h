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

namespace srslog {
class log_channel;
}

namespace srsran {
namespace app_helpers {

struct metrics_config;

/// Returns the metrics log channel for the logger type.
srslog::log_channel& fetch_logger_metrics_log_channel();

/// Returns the metrics log channel for the JSON type.
srslog::log_channel& fetch_json_metrics_log_channel();

/// Initializes the metrics log channels using the given configuration.
void initialize_metrics_log_channels(const metrics_config& config, unsigned hex_dump_size);

} // namespace app_helpers
} // namespace srsran
