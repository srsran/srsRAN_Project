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

#include "du_high_rlc_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/du_high/rlc.h"
#include "du_high_rlc_metrics.h"

using namespace srsran;

void rlc_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const rlc_metrics& drb = static_cast<const rlc_metrics_impl&>(metric).get_metrics();

  log_chan("{}", app_helpers::json_generators::generate_string(drb, 2));
}

void rlc_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const rlc_metrics& drb = static_cast<const rlc_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "RLC Metrics:");
  fmt::format_to(std::back_inserter(buffer), " du={}", static_cast<uint32_t>(drb.du_index));
  fmt::format_to(std::back_inserter(buffer), " ue={}", static_cast<uint32_t>(drb.ue_index));
  fmt::format_to(std::back_inserter(buffer), " rb={}", drb.rb_id);
  fmt::format_to(std::back_inserter(buffer), " mode={} TX=[", drb.rx.mode);
  format_rlc_tx_metrics(buffer, drb.metrics_period, drb.tx);
  fmt::format_to(std::back_inserter(buffer), "] RX=[");
  format_rlc_rx_metrics(buffer, drb.metrics_period, drb.rx);
  fmt::format_to(std::back_inserter(buffer), "]");
  log_chan("{}", to_c_str(buffer));
}

void rlc_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const rlc_metrics_impl&>(metric).get_metrics());
}
