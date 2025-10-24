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

#include "mac_metrics_consumers.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

static void write_latency_information(fmt::memory_buffer&                              buffer,
                                      const mac_dl_cell_metric_report::latency_report& report,
                                      std::string_view                                 name,
                                      bool                                             add_space = true)
{
  fmt::format_to(std::back_inserter(buffer),
                 "{}=[avg={}usec max={}usec max_slot={}]",
                 name,
                 std::round(report.average.count() * 1e-3),
                 std::round(report.max.count() * 1e-3),
                 report.max_slot);

  if (add_space) {
    fmt::format_to(std::back_inserter(buffer), " ");
  }
}

void mac_metrics_consumer_log::handle_metric(const mac_dl_metric_report& report)
{
  fmt::memory_buffer buffer;

  for (unsigned i = 0, e = report.cells.size(); i != e; ++i) {
    const mac_dl_cell_metric_report& cell = report.cells[i];

    fmt::format_to(std::back_inserter(buffer),
                   "MAC cell pci={} metrics: nof_slots={} slot_duration={}usec nof_voluntary_context_switches={} "
                   "nof_involuntary_context_switches={} ",
                   static_cast<unsigned>(cell.pci),
                   cell.nof_slots,
                   std::round(cell.slot_duration.count() * 1e-3),
                   cell.count_voluntary_context_switches,
                   cell.count_involuntary_context_switches);

    write_latency_information(buffer, cell.wall_clock_latency, "wall_clock_latency");
    write_latency_information(buffer, cell.sched_latency, "sched_latency");
    write_latency_information(buffer, cell.dl_tti_req_latency, "dl_tti_req_latency");
    write_latency_information(buffer, cell.tx_data_req_latency, "tx_data_req_latency");
    write_latency_information(buffer, cell.ul_tti_req_latency, "ul_tti_req_latency");
    write_latency_information(buffer, cell.slot_ind_dequeue_latency, "slot_ind_dequeue_latency");
    write_latency_information(buffer, cell.slot_ind_msg_time_diff, "slot_ind_msg_time_diff", false);

    log_chan("{}", to_c_str(buffer));
    buffer.clear();
  }
}
