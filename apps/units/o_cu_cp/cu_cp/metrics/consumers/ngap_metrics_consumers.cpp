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

using namespace srsran;

void ngap_metrics_consumer_log::handle_metric(const std::vector<ngap_info>&      report,
                                              const mobility_management_metrics& mobility_metrics)
{
  if (report.empty()) {
    return;
  }

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "CU-CP NGAP metrics: {}", format_ngap_metrics(report, mobility_metrics));

  log_chan("{}", to_c_str(buffer));
  buffer.clear();
}
