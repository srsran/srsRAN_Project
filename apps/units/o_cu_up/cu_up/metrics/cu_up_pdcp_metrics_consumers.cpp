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

#include "cu_up_pdcp_metrics_consumers.h"
#include "cu_up_pdcp_metrics.h"
#include "srsran/pdcp/pdcp_metrics.h"

using namespace srsran;

namespace {

DECLARE_METRIC("average_latency_us", metric_average_latency, double, "us");
DECLARE_METRIC("min_latency_us", metric_min_latency, double, "us");
DECLARE_METRIC("max_latency_us", metric_max_latency, double, "us");
DECLARE_METRIC("average_throughput_Mbps", metric_avg_throughput_mbps, double, "Mbps");
DECLARE_METRIC("cpu_usage_percent", metric_cpu_usage, double, "");

/// PDCP DL processing metrics.
DECLARE_METRIC_SET("dl",
                   mset_dl_pdcp,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage);

/// PDCP UL processing metrics.
DECLARE_METRIC_SET("ul",
                   mset_ul_pdcp,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage);

DECLARE_METRIC_SET("pdcp", mset_pdcp, mset_dl_pdcp, mset_ul_pdcp);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_SET("cu-up", mset_cu_up, mset_pdcp);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, mset_cu_up>;

} // namespace

void cu_up_pdcp_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const cu_up_pdcp_metrics_impl&>(metric).get_metrics());
}

cu_up_pdcp_metrics_consumer_json::cu_up_pdcp_metrics_consumer_json(srslog::basic_logger& logger_,
                                                                   srslog::log_channel&  log_chan_,
                                                                   task_executor&        executor_,
                                                                   unique_timer          timer_,
                                                                   unsigned              report_period_ms_) :
  report_period_ms(report_period_ms_),
  logger(logger_),
  log_chan(log_chan_),
  executor(executor_),
  timer(std::move(timer_))
{
  srsran_assert(report_period_ms > 10, "CU-UP report period is too fast to work with current JSON consumer");
  srsran_assert(timer.is_valid(), "Invalid timer passed to metrics controller");

  // Shift the timer a little.
  timer.set(std::chrono::milliseconds(report_period_ms / 10), [this](timer_id_t tid) { initialize_timer(); });
  timer.run();
}

void cu_up_pdcp_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const pdcp_metrics_container& pdcp_metric = static_cast<const cu_up_pdcp_metrics_impl&>(metric).get_metrics();

  // Tx aggregation.
  const pdcp_tx_metrics_container& tx_metric = pdcp_metric.tx;
  pdcp_tx_metrics_container&       aggr_tx   = aggr_metrics.tx;
  aggr_tx.num_sdus += tx_metric.num_sdus;
  aggr_tx.num_sdu_bytes += tx_metric.num_sdu_bytes;
  aggr_tx.num_pdus += tx_metric.num_pdus;
  aggr_tx.num_pdu_bytes += tx_metric.num_pdu_bytes;
  aggr_tx.num_discard_timeouts += tx_metric.num_discard_timeouts;

  aggr_tx.sum_pdu_latency_ns += tx_metric.sum_pdu_latency_ns;
  aggr_tx.sum_crypto_processing_latency_ns += tx_metric.sum_crypto_processing_latency_ns;

  if (tx_metric.min_pdu_latency_ns) {
    aggr_tx.min_pdu_latency_ns =
        std::min(aggr_tx.min_pdu_latency_ns.value_or(UINT32_MAX), tx_metric.min_pdu_latency_ns.value());
  }
  if (tx_metric.max_pdu_latency_ns) {
    aggr_tx.max_pdu_latency_ns = std::max(aggr_tx.max_pdu_latency_ns.value_or(0), tx_metric.max_pdu_latency_ns.value());
  }

  aggr_metrics.tx_cpu_usage += tx_metric.sum_crypto_processing_latency_ns /
                               (static_cast<double>(pdcp_metric.metrics_period.count()) * 1e6) * 100.0;

  // Rx aggregation.
  const pdcp_rx_metrics_container& rx_metric = pdcp_metric.rx;
  pdcp_rx_metrics_container&       aggr_rx   = aggr_metrics.rx;

  aggr_rx.num_pdus += rx_metric.num_pdus;
  aggr_rx.num_pdu_bytes += rx_metric.num_pdu_bytes;
  aggr_rx.num_data_pdus += rx_metric.num_data_pdus;
  aggr_rx.num_data_pdu_bytes += rx_metric.num_data_pdu_bytes;
  aggr_rx.num_dropped_pdus += rx_metric.num_dropped_pdus;
  aggr_rx.num_sdus += rx_metric.num_sdus;
  aggr_rx.num_sdu_bytes += rx_metric.num_sdu_bytes;
  aggr_rx.num_integrity_verified_pdus += rx_metric.num_integrity_verified_pdus;
  aggr_rx.num_integrity_failed_pdus += rx_metric.num_integrity_failed_pdus;
  aggr_rx.num_t_reordering_timeouts += rx_metric.num_t_reordering_timeouts;
  aggr_rx.reordering_counter += rx_metric.reordering_counter;

  aggr_rx.reordering_delay_us += rx_metric.reordering_delay_us;
  aggr_rx.sum_sdu_latency_ns += rx_metric.sum_sdu_latency_ns;
  aggr_rx.sum_crypto_processing_latency_ns += rx_metric.sum_crypto_processing_latency_ns;

  if (rx_metric.min_sdu_latency_ns) {
    aggr_rx.min_sdu_latency_ns =
        std::min(aggr_rx.min_sdu_latency_ns.value_or(UINT32_MAX), rx_metric.min_sdu_latency_ns.value());
  }
  if (rx_metric.max_sdu_latency_ns) {
    aggr_rx.max_sdu_latency_ns = std::max(aggr_rx.max_sdu_latency_ns.value_or(0), rx_metric.max_sdu_latency_ns.value());
  }

  aggr_metrics.rx_cpu_usage += rx_metric.sum_crypto_processing_latency_ns /
                               (static_cast<double>(pdcp_metric.metrics_period.count()) * 1e6) * 100.0;

  aggr_metrics.metrics_period = pdcp_metric.metrics_period;

  aggr_metrics.is_empty = false;
}

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

void cu_up_pdcp_metrics_consumer_json::print_metrics()
{
  if (aggr_metrics.is_empty) {
    return;
  }

  metric_context_t ctx("JSON CU-UP Metrics");

  // DL path.
  auto&                            dl_pdcp    = ctx.get<mset_cu_up>().get<mset_pdcp>().get<mset_dl_pdcp>();
  const pdcp_tx_metrics_container& tx_metrics = aggr_metrics.tx;
  double value = tx_metrics.num_pdus ? static_cast<double>(tx_metrics.sum_pdu_latency_ns) * 1e-3 /
                                           static_cast<double>(tx_metrics.num_pdus)
                                     : 0.0;
  dl_pdcp.write<metric_average_latency>(value);

  if (tx_metrics.min_pdu_latency_ns) {
    value = static_cast<double>(tx_metrics.min_pdu_latency_ns.value()) * 1e-3;
    dl_pdcp.write<metric_min_latency>(value);
  }

  if (tx_metrics.max_pdu_latency_ns) {
    value = static_cast<double>(tx_metrics.max_pdu_latency_ns.value()) * 1e-3;
    dl_pdcp.write<metric_max_latency>(value);
  }

  value = static_cast<double>(tx_metrics.num_pdu_bytes) * 8.0 * 1000.0 * 1e-6 /
          static_cast<double>(aggr_metrics.metrics_period.count());
  dl_pdcp.write<metric_avg_throughput_mbps>(value);

  dl_pdcp.write<metric_cpu_usage>(aggr_metrics.tx_cpu_usage);

  // UL path.
  auto&                            ul_pdcp    = ctx.get<mset_cu_up>().get<mset_pdcp>().get<mset_ul_pdcp>();
  const pdcp_rx_metrics_container& rx_metrics = aggr_metrics.rx;

  value = rx_metrics.num_sdus
              ? static_cast<double>(rx_metrics.sum_sdu_latency_ns) * 1e-3 / static_cast<double>(rx_metrics.num_sdus)
              : 0.0;
  ul_pdcp.write<metric_average_latency>(value);

  if (rx_metrics.min_sdu_latency_ns) {
    value = static_cast<double>(rx_metrics.min_sdu_latency_ns.value()) * 1e-3;
    ul_pdcp.write<metric_min_latency>(value);
  }

  if (rx_metrics.max_sdu_latency_ns) {
    value = static_cast<double>(rx_metrics.max_sdu_latency_ns.value()) * 1e-3;
    ul_pdcp.write<metric_max_latency>(value);
  }

  value = static_cast<double>(rx_metrics.num_pdu_bytes) * 8.0 * 1000.0 * 1e-6 /
          static_cast<double>(aggr_metrics.metrics_period.count());
  ul_pdcp.write<metric_avg_throughput_mbps>(value);

  ul_pdcp.write<metric_cpu_usage>(aggr_metrics.rx_cpu_usage);

  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);

  // Clear metrics after printing.
  clear_metrics();
}

void cu_up_pdcp_metrics_consumer_json::initialize_timer()
{
  timer.set(std::chrono::milliseconds(report_period_ms), [this](timer_id_t tid) {
    if (!executor.execute([this]() {
          print_metrics();
          timer.run();
        })) {
      logger.warning("Failed to enqueue task to print CU-UP metrics");
    }
  });
  timer.run();
}

void cu_up_pdcp_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
  const pdcp_metrics_container& pdcp_metric = static_cast<const cu_up_pdcp_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "PDCP Metrics: {}", pdcp_metric);
  log_chan("{}", to_c_str(buffer));
}
