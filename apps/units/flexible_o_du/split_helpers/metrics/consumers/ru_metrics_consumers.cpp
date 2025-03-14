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

#include "ru_metrics_consumers.h"
#include "consumer_helpers.h"
#include "srsran/ru/ru_metrics.h"

using namespace srsran;

namespace {

/// RU metrics.
DECLARE_METRIC("total", ofh_metrics_rx_total_pkts, unsigned, "");
DECLARE_METRIC("early", ofh_metrics_rx_early_pkts, unsigned, "");
DECLARE_METRIC("on_time", ofh_metrics_rx_on_time_pkts, unsigned, "");
DECLARE_METRIC("late", ofh_metrics_rx_late_pkts, unsigned, "");
DECLARE_METRIC_SET("received_packets",
                   mset_ofh_metrics_rx,
                   ofh_metrics_rx_total_pkts,
                   ofh_metrics_rx_early_pkts,
                   ofh_metrics_rx_on_time_pkts,
                   ofh_metrics_rx_late_pkts);

DECLARE_METRIC("pci", metric_pci, pci_t, "");

DECLARE_METRIC_SET("ul", ofh_metrics_ul, mset_ofh_metrics_rx);
DECLARE_METRIC_SET("cell", ofh_metrics_cell, metric_pci, ofh_metrics_ul);
DECLARE_METRIC_LIST("ofh", mlist_ofh_cells, std::vector<ofh_metrics_cell>);

DECLARE_METRIC_SET("ru", mset_ofh_ru, mlist_ofh_cells);
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");

/// OFH metrics context.
using ofh_metric_context_t = srslog::build_context_type<metric_timestamp_tag, mset_ofh_ru>;

} // namespace

static void
log_ru_ofh_metrics_json(srslog::log_channel& log_chan, const ofh::metrics& metrics, span<const pci_t> pci_sector_map)
{
  ofh_metric_context_t ctx("JSON RU-OFH Metrics");

  auto& ofh_cells = ctx.get<mset_ofh_ru>().get<mlist_ofh_cells>();

  srsran_assert(metrics.sectors.size() == pci_sector_map.size(),
                "OFH metrics number of sectors '{}' does not match number of cells '{}'",
                metrics.sectors.size(),
                pci_sector_map.size());

  for (const auto& cell_metrics : metrics.sectors) {
    const ofh::receiver_metrics& rx_metrics = cell_metrics.rx_metrics;

    auto& output = ofh_cells.emplace_back();
    output.write<metric_pci>(static_cast<unsigned>(pci_sector_map[cell_metrics.sector_id]));

    auto& ul_output = output.get<ofh_metrics_ul>().get<mset_ofh_metrics_rx>();
    ul_output.write<ofh_metrics_rx_total_pkts>(static_cast<unsigned>(
        rx_metrics.nof_early_messages + rx_metrics.nof_on_time_messages + rx_metrics.nof_late_messages));
    ul_output.write<ofh_metrics_rx_early_pkts>(static_cast<unsigned>(rx_metrics.nof_early_messages));
    ul_output.write<ofh_metrics_rx_on_time_pkts>(static_cast<unsigned>(rx_metrics.nof_on_time_messages));
    ul_output.write<ofh_metrics_rx_late_pkts>(static_cast<unsigned>(rx_metrics.nof_late_messages));
  }

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}

void ru_metrics_consumer_json::handle_metric(const ru_metrics& metric)
{
  if (auto* ofh_metrics = std::get_if<ofh::metrics>(&metric.metrics)) {
    log_ru_ofh_metrics_json(log_chan, *ofh_metrics, pci_sector_map);
  }
}

static void
log_ru_ofh_metrics(srslog::log_channel& log_chan, const ofh::metrics& metrics, span<const pci_t> pci_sector_map)
{
  for (const auto& cell_metrics : metrics.sectors) {
    const ofh::receiver_metrics& rx_metrics = cell_metrics.rx_metrics;
    log_chan("OFH sector#{} pci={} reception stats: rx_total={} rx_early={}, rx_on_time={}, rx_late={}",
             cell_metrics.sector_id,
             static_cast<unsigned>(pci_sector_map[cell_metrics.sector_id]),
             rx_metrics.nof_early_messages + rx_metrics.nof_on_time_messages + rx_metrics.nof_late_messages,
             rx_metrics.nof_early_messages,
             rx_metrics.nof_on_time_messages,
             rx_metrics.nof_late_messages);
  }
}

void ru_metrics_consumer_log::handle_metric(const ru_metrics& metric)
{
  if (auto* ofh_metrics = std::get_if<ofh::metrics>(&metric.metrics)) {
    log_ru_ofh_metrics(log_chan, *ofh_metrics, pci_sector_map);
  }
}
