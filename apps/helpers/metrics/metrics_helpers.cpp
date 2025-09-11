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

#include "metrics_helpers.h"
#include "metrics_config.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace app_helpers;

/// Log channel name for the logger type.
static const std::string logger_channel_name = "METRICS";
/// Log channel name for the JSON type.
static const std::string json_channel_name = "JSON_channel";

srslog::log_channel& app_helpers::fetch_logger_metrics_log_channel()
{
  return srslog::fetch_log_channel(logger_channel_name);
}

srslog::log_channel& app_helpers::fetch_json_metrics_log_channel()
{
  return srslog::fetch_log_channel(json_channel_name);
}

static void initialize_logger_channel(unsigned hex_dump_size)
{
  srslog::log_channel& channel = srslog::fetch_log_channel(logger_channel_name, {logger_channel_name, {}, {}});
  channel.set_hex_dump_max_size(hex_dump_size);
  channel.set_enabled(true);
}

void app_helpers::initialize_metrics_log_channels(const metrics_config& config, unsigned hex_dump_size)
{
  if (config.enable_log_metrics) {
    initialize_logger_channel(hex_dump_size);
  }
}
