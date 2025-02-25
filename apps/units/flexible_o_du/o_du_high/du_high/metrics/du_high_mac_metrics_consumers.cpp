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

#include "du_high_mac_metrics_consumers.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/ran/pci.h"

using namespace srsran;

namespace {

DECLARE_METRIC("pci", metric_pci, pci_t, "");
DECLARE_METRIC("average_latency_us", metric_average_latency, double, "us");
DECLARE_METRIC("min_latency_us", metric_min_latency, double, "us");
DECLARE_METRIC("max_latency_us", metric_max_latency, double, "us");
DECLARE_METRIC("cpu_usage_percent", metric_cpu_usage, double, "");

DECLARE_METRIC_SET("cell",
                   mset_dl_cell,
                   metric_pci,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_cpu_usage);

DECLARE_METRIC_LIST("dl", mlist_dl, std::vector<mset_dl_cell>);
DECLARE_METRIC_SET("mac", mset_mac, mlist_dl);
DECLARE_METRIC_SET("du_high", mset_du_high, mset_mac);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_SET("du", mset_du, mset_du_high);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, mset_du>;

} // namespace

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

void mac_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const auto& mac_metrics = static_cast<const mac_metrics_impl&>(metric).get_metrics();

  metric_context_t ctx("JSON MAC Metrics");

  auto& dl_cells = ctx.get<mset_du>().get<mset_du_high>().get<mset_mac>().get<mlist_dl>();

  for (const auto& report : mac_metrics.dl.cells) {
    double metrics_period = (report.slot_duration * report.nof_slots).count();
    double cpu_usage      = 100.0 * static_cast<double>(report.wall_clock_latency.average.count()) / metrics_period;

    auto& output = dl_cells.emplace_back();
    output.write<metric_pci>(static_cast<unsigned>(report.pci));
    output.write<metric_average_latency>(static_cast<double>(report.wall_clock_latency.average.count()) / 1000.0);
    output.write<metric_min_latency>(static_cast<double>(report.wall_clock_latency.min.count()) / 1000.0);
    output.write<metric_max_latency>(static_cast<double>(report.wall_clock_latency.max.count()) / 1000.0);
    output.write<metric_cpu_usage>(cpu_usage);
  }

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}
