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
