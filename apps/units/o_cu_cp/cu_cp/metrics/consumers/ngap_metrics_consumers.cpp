/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_metrics_consumers.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

void ngap_metrics_consumer_log::handle_metric(const std::vector<ngap_info>&      report,
                                              const mobility_management_metrics& mobility_metrics)
{
  if (report.empty()) {
    return;
  }

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "CU-CP NGAP metrics:");

  for (const auto& ngap_info : report) {
    // log ngap metrics
    fmt::format_to(std::back_inserter(buffer), " [");
    fmt::format_to(std::back_inserter(buffer), " amf_name={}", ngap_info.amf_name);

    for (const auto& pdu_session_metric : ngap_info.metrics.pdu_session_metrics) {
      fmt::format_to(std::back_inserter(buffer),
                     " s-nssai=(sst={} sd={})",
                     pdu_session_metric.first.sst.value(),
                     pdu_session_metric.first.sd.is_set() ? fmt::format("{}", pdu_session_metric.first.sd.value())
                                                          : "na");

      fmt::format_to(std::back_inserter(buffer),
                     " nof_pdu_sessions_requested_to_setup={} nof_pdu_sessions_successfully_setup={}",
                     pdu_session_metric.second.nof_pdu_sessions_requested_to_setup,
                     pdu_session_metric.second.nof_pdu_sessions_successfully_setup);

      fmt::format_to(std::back_inserter(buffer), " nof_pdu_sessions_failed_to_setup=[");
      unsigned cause_index = 0;
      for (const auto& cause_count : pdu_session_metric.second.nof_pdu_sessions_failed_to_setup) {
        fmt::format_to(std::back_inserter(buffer),
                       " {}={}",
                       pdu_session_metric.second.nof_pdu_sessions_failed_to_setup.get_cause(cause_index),
                       cause_count);
        ++cause_index;
      }
      fmt::format_to(std::back_inserter(buffer), " ]");
    }
    fmt::format_to(std::back_inserter(buffer),
                   " nof_cn_initiated_paging_requests={}",
                   ngap_info.metrics.nof_cn_initiated_paging_requests);

    fmt::format_to(std::back_inserter(buffer), " ],");
  }

  fmt::format_to(std::back_inserter(buffer),
                 " nof_handover_preparations_requested={} nof_successful_handover_preparations={} ",
                 mobility_metrics.nof_handover_preparations_requested,
                 mobility_metrics.nof_successful_handover_preparations);

  log_chan("{}", to_c_str(buffer));
  buffer.clear();
}
