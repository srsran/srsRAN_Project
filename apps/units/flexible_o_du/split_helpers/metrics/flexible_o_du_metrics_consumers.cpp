/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "flexible_o_du_metrics_consumers.h"
#include "flexible_o_du_app_service_metrics.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

void flexible_o_du_metrics_consumer_dummy::handle_metric(const app_services::metrics_set& metric)
{
  // Implement me!
}

flexible_o_du_metrics_consumer_log::flexible_o_du_metrics_consumer_log(srslog::basic_logger& logger_) : logger(logger_)
{
}

void flexible_o_du_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const flexible_o_du_metrics& odu_metrics =
      static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;

  const auto& m_ldpc_decoder = odu_metrics.du.low.ldpc_metrics.decoder_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- LDPC Decoder metrics (per codeblock) ---\n");
  fmt::format_to(std::back_inserter(buffer), "        CPU usage: {:.2f} %\n", m_ldpc_decoder.cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "     Avg. CB size: {:.2f} bits\n", m_ldpc_decoder.avg_cb_size);
  fmt::format_to(std::back_inserter(buffer), "     Avg. latency: {:.2f} us\n", m_ldpc_decoder.avg_cb_latency_us);
  fmt::format_to(std::back_inserter(buffer), "  Avg. iterations: {:.2f} iter\n", m_ldpc_decoder.avg_nof_iterations);
  fmt::format_to(std::back_inserter(buffer), "      Decode rate: {:.2f} Mbps\n", m_ldpc_decoder.decoding_rate_Mbps);

  logger.info("{}", to_c_str(buffer));
}
