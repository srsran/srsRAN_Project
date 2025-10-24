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

#include "apps/helpers/metrics/helpers.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics.h"
#include "apps/services/metrics/metrics_consumer.h"
#include "external/nlohmann/json.hpp"
#include "srsran/srslog/log_channel.h"

namespace srsran {
namespace app_services {

/// Log consumer for the buffer pool metrics.
class buffer_pool_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit buffer_pool_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    size_t central_cache_size = static_cast<const buffer_pool_metrics_impl&>(metric).get_central_cache_size();

    log_chan("Buffer pool: central_cache_size={} segments", central_cache_size);
  }

private:
  srslog::log_channel& log_chan;
};

/// JSON consumer for the buffer pool metrics.
class buffer_pool_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit buffer_pool_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    nlohmann::json json;

    size_t central_cache_size = static_cast<const buffer_pool_metrics_impl&>(metric).get_central_cache_size();

    json["timestamp"]                      = app_helpers::get_time_stamp();
    auto& json_buffer_pool                 = json["buffer_pool"];
    json_buffer_pool["central_cache_size"] = central_cache_size;

    log_chan("{}", json.dump(-1));
  }

private:
  srslog::log_channel& log_chan;
};

} // namespace app_services
} // namespace srsran
