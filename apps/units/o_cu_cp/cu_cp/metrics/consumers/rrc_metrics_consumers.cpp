/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_metrics_consumers.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

void rrc_metrics_consumer_log::handle_metric(const std::vector<cu_cp_metrics_report::du_info>& report,
                                             const mobility_management_metrics&                mobility_metrics)
{
  if (report.empty()) {
    return;
  }

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "CU-CP RRC metrics:");

  for (const auto& du_info : report) {
    // log RRC metrics
    fmt::format_to(std::back_inserter(buffer), " [");
    fmt::format_to(std::back_inserter(buffer), " gnb_du_id={}", gnb_du_id_to_int(du_info.id));

    fmt::format_to(std::back_inserter(buffer),
                   " mean_nof_rrc_connections={} max_nof_rrc_connections={}",
                   du_info.rrc_metrics.mean_nof_rrc_connections,
                   du_info.rrc_metrics.max_nof_rrc_connections);

    fmt::format_to(std::back_inserter(buffer), " attempted_rrc_connection_establishments=[");
    unsigned cause_index = 0;
    for (const auto& cause_count : du_info.rrc_metrics.attempted_rrc_connection_establishments) {
      fmt::format_to(std::back_inserter(buffer),
                     " {}={}",
                     du_info.rrc_metrics.attempted_rrc_connection_establishments.get_cause(cause_index),
                     cause_count);

      ++cause_index;
    }
    fmt::format_to(std::back_inserter(buffer), " ]");

    fmt::format_to(std::back_inserter(buffer), " successful_rrc_connection_establishments=[");
    cause_index = 0;
    for (const auto& cause_count : du_info.rrc_metrics.successful_rrc_connection_establishments) {
      fmt::format_to(std::back_inserter(buffer),
                     " {}={}",
                     du_info.rrc_metrics.successful_rrc_connection_establishments.get_cause(cause_index),
                     cause_count);
      ++cause_index;
    }
    fmt::format_to(std::back_inserter(buffer), " ]");

    fmt::format_to(
        std::back_inserter(buffer),
        " attempted_rrc_connection_reestablishments={} successful_rrc_connection_reestablishments_with_ue_context={} "
        "successful_rrc_connection_reestablishments_without_ue_context={}",
        du_info.rrc_metrics.attempted_rrc_connection_reestablishments,
        du_info.rrc_metrics.successful_rrc_connection_reestablishments_with_ue_context,
        du_info.rrc_metrics.successful_rrc_connection_reestablishments_without_ue_context);
    fmt::format_to(std::back_inserter(buffer), " ],");
  }

  fmt::format_to(std::back_inserter(buffer),
                 " nof_handover_executions_requested={} nof_successful_handover_executions={} ",
                 mobility_metrics.nof_handover_executions_requested,
                 mobility_metrics.nof_successful_handover_executions);

  log_chan("{}", to_c_str(buffer));
  buffer.clear();
}
