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
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/units.h"

using namespace srsran;

namespace {

/// RU metrics.
DECLARE_METRIC("total", ofh_metrics_rx_total_pkts, unsigned, "");
DECLARE_METRIC("early", ofh_metrics_rx_early_pkts, unsigned, "");
DECLARE_METRIC("on_time", ofh_metrics_rx_on_time_pkts, unsigned, "");
DECLARE_METRIC("late", ofh_metrics_rx_late_pkts, unsigned, "");
DECLARE_METRIC_SET("received_packets",
                   mset_ofh_metrics_rx_kpis,
                   ofh_metrics_rx_total_pkts,
                   ofh_metrics_rx_early_pkts,
                   ofh_metrics_rx_on_time_pkts,
                   ofh_metrics_rx_late_pkts);

DECLARE_METRIC("average_latency_us", metric_average_latency, float, "us");
DECLARE_METRIC("max_latency_us", metric_max_latency, float, "us");
DECLARE_METRIC("average_throughput_Mbps", metric_avg_throughput_mbps, float, "Mbps");
DECLARE_METRIC("cpu_usage_percent", metric_cpu_usage, float, "");

DECLARE_METRIC_SET("ethernet_receiver",
                   mset_ofh_metrics_ether_rx,
                   metric_avg_throughput_mbps,
                   metric_average_latency,
                   metric_max_latency,
                   metric_cpu_usage);
DECLARE_METRIC_SET("ethernet_transmitter",
                   mset_ofh_metrics_ether_tx,
                   metric_avg_throughput_mbps,
                   metric_average_latency,
                   metric_max_latency,
                   metric_cpu_usage);

DECLARE_METRIC_SET("prach",
                   mset_ofh_metrics_prach_decoder,
                   metric_average_latency,
                   metric_max_latency,
                   metric_cpu_usage);
DECLARE_METRIC_SET("data", mset_ofh_metrics_ul_decoder, metric_average_latency, metric_max_latency, metric_cpu_usage);
DECLARE_METRIC_SET("message_decoder",
                   mset_ofh_metrics_message_decoder,
                   mset_ofh_metrics_prach_decoder,
                   mset_ofh_metrics_ul_decoder);

DECLARE_METRIC_SET("dl_cp",
                   mset_ofh_metrics_dl_cp_encoder,
                   metric_average_latency,
                   metric_max_latency,
                   metric_cpu_usage);
DECLARE_METRIC_SET("ul_cp",
                   mset_ofh_metrics_ul_cp_encoder,
                   metric_average_latency,
                   metric_max_latency,
                   metric_cpu_usage);
DECLARE_METRIC_SET("dl_up",
                   mset_ofh_metrics_dl_up_encoder,
                   metric_average_latency,
                   metric_max_latency,
                   metric_cpu_usage);
DECLARE_METRIC_SET("message_encoder",
                   mset_ofh_metrics_message_encoder,
                   mset_ofh_metrics_dl_cp_encoder,
                   mset_ofh_metrics_ul_cp_encoder,
                   mset_ofh_metrics_dl_up_encoder);

DECLARE_METRIC_SET("message_transmitter",
                   mset_ofh_metrics_message_transmitter,
                   metric_average_latency,
                   metric_max_latency,
                   metric_cpu_usage);

DECLARE_METRIC("late_dl_grids", ofh_metrics_tx_late_dl_grids, unsigned, "");
DECLARE_METRIC("late_ul_requests", ofh_metrics_tx_late_ul_req, unsigned, "");
DECLARE_METRIC_SET("transmitter_stats",
                   mset_ofh_metrics_tx_kpis,
                   ofh_metrics_tx_late_dl_grids,
                   ofh_metrics_tx_late_ul_req);

DECLARE_METRIC("pci", metric_pci, pci_t, "");

DECLARE_METRIC_SET("ul",
                   ofh_metrics_ul,
                   mset_ofh_metrics_rx_kpis,
                   mset_ofh_metrics_ether_rx,
                   mset_ofh_metrics_message_decoder);
DECLARE_METRIC_SET("dl",
                   ofh_metrics_dl,
                   mset_ofh_metrics_ether_tx,
                   mset_ofh_metrics_message_encoder,
                   mset_ofh_metrics_message_transmitter,
                   mset_ofh_metrics_tx_kpis);
DECLARE_METRIC_SET("cell", ofh_metrics_cell, metric_pci, ofh_metrics_ul, ofh_metrics_dl);
DECLARE_METRIC_LIST("ofh", mlist_ofh_cells, std::vector<ofh_metrics_cell>);

DECLARE_METRIC_SET("ru", mset_ofh_ru, mlist_ofh_cells);
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");

/// OFH metrics context.
using ofh_metric_context_t = srslog::build_context_type<metric_timestamp_tag, mset_ofh_ru>;

} // namespace

/// Size in bytes of the buffer used to prepare the metrics before logging.
static constexpr unsigned str_buffer_size = 2048;

/// Return the given value if it is not a Nan or Inf, otherwise returns 0.
static double validate_fp_value(double value)
{
  if (!std::isnan(value) && !std::isinf(value)) {
    return value;
  }
  return 0.0;
}

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
    const ofh::received_messages_metrics& rx_metrics = cell_metrics.rx_metrics.rx_messages_metrics;

    auto& output = ofh_cells.emplace_back();
    output.write<metric_pci>(static_cast<unsigned>(pci_sector_map[cell_metrics.sector_id]));

    // Received messages KPIs.
    auto& ul_rx_kpis = output.get<ofh_metrics_ul>().get<mset_ofh_metrics_rx_kpis>();
    ul_rx_kpis.write<ofh_metrics_rx_total_pkts>(static_cast<unsigned>(
        rx_metrics.nof_early_messages + rx_metrics.nof_on_time_messages + rx_metrics.nof_late_messages));
    ul_rx_kpis.write<ofh_metrics_rx_early_pkts>(static_cast<unsigned>(rx_metrics.nof_early_messages));
    ul_rx_kpis.write<ofh_metrics_rx_on_time_pkts>(static_cast<unsigned>(rx_metrics.nof_on_time_messages));
    ul_rx_kpis.write<ofh_metrics_rx_late_pkts>(static_cast<unsigned>(rx_metrics.nof_late_messages));

    // Ethernet receiver metrics.
    float ether_rx_cpu_usage = validate_fp_value(cell_metrics.rx_metrics.eth_receiver_metrics.cpu_usage_us /
                                                 (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);

    float ether_rx_throughput =
        validate_fp_value(static_cast<float>(cell_metrics.rx_metrics.eth_receiver_metrics.total_nof_bytes * CHAR_BIT) /
                          static_cast<float>(cell_metrics.metrics_period_ms.count() * 1e3));

    auto& ul_ether_rx = output.get<ofh_metrics_ul>().get<mset_ofh_metrics_ether_rx>();
    ul_ether_rx.write<metric_avg_throughput_mbps>(ether_rx_throughput);
    ul_ether_rx.write<metric_average_latency>(
        validate_fp_value(cell_metrics.rx_metrics.eth_receiver_metrics.avg_packet_rx_latency_us));
    ul_ether_rx.write<metric_max_latency>(
        validate_fp_value(cell_metrics.rx_metrics.eth_receiver_metrics.max_packet_rx_latency_us));
    ul_ether_rx.write<metric_cpu_usage>(ether_rx_cpu_usage);

    // Message decoder metrics.
    const auto& ul_prach_df_metrics = cell_metrics.rx_metrics.rx_decoding_perf_metrics.prach_processing_metrics;
    const auto& ul_data_df_metrics  = cell_metrics.rx_metrics.rx_decoding_perf_metrics.data_processing_metrics;
    float       prach_unpacking_cpu_usage =
        validate_fp_value(ul_prach_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);
    float data_unpacking_cpu_usage =
        validate_fp_value(ul_data_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);

    auto& ul_prach_message_decoder =
        output.get<ofh_metrics_ul>().get<mset_ofh_metrics_message_decoder>().get<mset_ofh_metrics_prach_decoder>();
    ul_prach_message_decoder.write<metric_average_latency>(
        validate_fp_value(ul_prach_df_metrics.message_unpacking_avg_latency_us));
    ul_prach_message_decoder.write<metric_max_latency>(
        validate_fp_value(ul_prach_df_metrics.message_unpacking_max_latency_us));
    ul_prach_message_decoder.write<metric_cpu_usage>(prach_unpacking_cpu_usage);

    auto& ul_data_message_decoder =
        output.get<ofh_metrics_ul>().get<mset_ofh_metrics_message_decoder>().get<mset_ofh_metrics_ul_decoder>();
    ul_data_message_decoder.write<metric_average_latency>(
        validate_fp_value(ul_data_df_metrics.message_unpacking_avg_latency_us));
    ul_data_message_decoder.write<metric_max_latency>(
        validate_fp_value(ul_data_df_metrics.message_unpacking_max_latency_us));
    ul_data_message_decoder.write<metric_cpu_usage>(data_unpacking_cpu_usage);

    // Ethernet transmitter metrics.
    float ether_tx_cpu_usage = validate_fp_value(cell_metrics.tx_metrics.eth_transmitter_metrics.cpu_usage_us /
                                                 (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);

    float ether_tx_throughput = validate_fp_value(
        static_cast<float>(cell_metrics.tx_metrics.eth_transmitter_metrics.total_nof_bytes * CHAR_BIT) /
        static_cast<float>(cell_metrics.metrics_period_ms.count() * 1e3));

    auto& dl_ether_tx = output.get<ofh_metrics_dl>().get<mset_ofh_metrics_ether_tx>();
    dl_ether_tx.write<metric_avg_throughput_mbps>(ether_tx_throughput);
    dl_ether_tx.write<metric_average_latency>(
        validate_fp_value(cell_metrics.tx_metrics.eth_transmitter_metrics.avg_packet_tx_latency_us));
    dl_ether_tx.write<metric_max_latency>(
        validate_fp_value(cell_metrics.tx_metrics.eth_transmitter_metrics.max_packet_tx_latency_us));
    dl_ether_tx.write<metric_cpu_usage>(ether_tx_cpu_usage);

    // Message encoder metrics.
    // DL-CP.
    const auto& dl_cp_df_metrics = cell_metrics.tx_metrics.dl_metrics.dl_cp_metrics;
    float       dl_cp_cpu_usage =
        validate_fp_value(dl_cp_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);

    auto& dl_cp_message_encoder =
        output.get<ofh_metrics_dl>().get<mset_ofh_metrics_message_encoder>().get<mset_ofh_metrics_dl_cp_encoder>();
    dl_cp_message_encoder.write<metric_average_latency>(
        validate_fp_value(dl_cp_df_metrics.message_packing_avg_latency_us));
    dl_cp_message_encoder.write<metric_max_latency>(validate_fp_value(dl_cp_df_metrics.message_packing_max_latency_us));
    dl_cp_message_encoder.write<metric_cpu_usage>(dl_cp_cpu_usage);

    // UL-CP.
    const auto& ul_cp_df_metrics = cell_metrics.tx_metrics.ul_metrics.ul_cp_metrics;
    float       ul_cp_cpu_usage =
        validate_fp_value(ul_cp_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);

    auto& ul_cp_message_encoder =
        output.get<ofh_metrics_dl>().get<mset_ofh_metrics_message_encoder>().get<mset_ofh_metrics_ul_cp_encoder>();
    ul_cp_message_encoder.write<metric_average_latency>(
        validate_fp_value(ul_cp_df_metrics.message_packing_avg_latency_us));
    ul_cp_message_encoder.write<metric_max_latency>(validate_fp_value(ul_cp_df_metrics.message_packing_max_latency_us));
    ul_cp_message_encoder.write<metric_cpu_usage>(ul_cp_cpu_usage);

    // DL-UP.
    const auto& dl_up_df_metrics = cell_metrics.tx_metrics.dl_metrics.dl_up_metrics;
    float       dl_up_cpu_usage =
        validate_fp_value(dl_up_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);

    auto& dl_up_message_encoder =
        output.get<ofh_metrics_dl>().get<mset_ofh_metrics_message_encoder>().get<mset_ofh_metrics_dl_up_encoder>();
    dl_up_message_encoder.write<metric_average_latency>(
        validate_fp_value(dl_up_df_metrics.message_packing_avg_latency_us));
    dl_up_message_encoder.write<metric_max_latency>(validate_fp_value(dl_up_df_metrics.message_packing_max_latency_us));
    dl_up_message_encoder.write<metric_cpu_usage>(dl_up_cpu_usage);

    // Message transmitter.
    const auto& message_tx_metrics = cell_metrics.tx_metrics.message_tx_metrics;
    float       msg_tx_cpu_usage =
        validate_fp_value(message_tx_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f);

    auto& message_transmitter = output.get<ofh_metrics_dl>().get<mset_ofh_metrics_message_transmitter>();
    message_transmitter.write<metric_average_latency>(validate_fp_value(message_tx_metrics.message_tx_avg_latency_us));
    message_transmitter.write<metric_max_latency>(validate_fp_value(message_tx_metrics.message_tx_max_latency_us));
    message_transmitter.write<metric_cpu_usage>(msg_tx_cpu_usage);

    // Transmitter KPIs.
    auto& transmitter_kpis = output.get<ofh_metrics_dl>().get<mset_ofh_metrics_tx_kpis>();
    transmitter_kpis.write<ofh_metrics_tx_late_dl_grids>(cell_metrics.tx_metrics.dl_metrics.nof_late_dl_grids);
    transmitter_kpis.write<ofh_metrics_tx_late_ul_req>(cell_metrics.tx_metrics.ul_metrics.nof_late_ul_requests);
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

static void log_ru_ofh_performance_metrics_verbose(fmt::basic_memory_buffer<char, str_buffer_size>& buffer,
                                                   const ofh::sector_metrics&                       cell_metrics,
                                                   span<const pci_t>                                pci_sector_map)
{
  float ether_rx_cpu_usage = cell_metrics.rx_metrics.eth_receiver_metrics.cpu_usage_us /
                             (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;
  float ether_tx_cpu_usage = cell_metrics.tx_metrics.eth_transmitter_metrics.cpu_usage_us /
                             (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;
  float ether_rx_throughput =
      static_cast<float>(cell_metrics.rx_metrics.eth_receiver_metrics.total_nof_bytes * CHAR_BIT) /
      static_cast<float>(cell_metrics.metrics_period_ms.count() * 1e3);
  float ether_tx_throughput =
      static_cast<float>(cell_metrics.tx_metrics.eth_transmitter_metrics.total_nof_bytes * CHAR_BIT) /
      static_cast<float>(cell_metrics.metrics_period_ms.count() * 1e3);

  fmt::format_to(std::back_inserter(buffer),
                 "; {} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us throughput={:.2f}Mbps "
                 "rx_bytes={}; ",
                 "ether_rx:",
                 validate_fp_value(ether_rx_cpu_usage),
                 validate_fp_value(cell_metrics.rx_metrics.eth_receiver_metrics.max_packet_rx_latency_us),
                 validate_fp_value(cell_metrics.rx_metrics.eth_receiver_metrics.avg_packet_rx_latency_us),
                 validate_fp_value(ether_rx_throughput),
                 cell_metrics.rx_metrics.eth_receiver_metrics.total_nof_bytes);
  fmt::format_to(std::back_inserter(buffer),
                 "{} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us throughput={:.2f}Mbps "
                 "tx_bytes={}; ",
                 "ether_tx:",
                 validate_fp_value(ether_tx_cpu_usage),
                 validate_fp_value(cell_metrics.tx_metrics.eth_transmitter_metrics.max_packet_tx_latency_us),
                 validate_fp_value(cell_metrics.tx_metrics.eth_transmitter_metrics.avg_packet_tx_latency_us),
                 validate_fp_value(ether_tx_throughput),
                 cell_metrics.tx_metrics.eth_transmitter_metrics.total_nof_bytes);

  const auto& ul_prach_df_metrics = cell_metrics.rx_metrics.rx_decoding_perf_metrics.prach_processing_metrics;
  const auto& ul_data_df_metrics  = cell_metrics.rx_metrics.rx_decoding_perf_metrics.data_processing_metrics;

  float prach_unpacking_cpu_usage =
      ul_prach_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;
  float data_unpacking_cpu_usage =
      ul_data_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;

  fmt::format_to(std::back_inserter(buffer),
                 "{} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us; ",
                 "rcv_prach:",
                 validate_fp_value(prach_unpacking_cpu_usage),
                 validate_fp_value(ul_prach_df_metrics.message_unpacking_max_latency_us),
                 validate_fp_value(ul_prach_df_metrics.message_unpacking_avg_latency_us));
  fmt::format_to(std::back_inserter(buffer),
                 "{} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us; ",
                 "rcv_ul:",
                 validate_fp_value(data_unpacking_cpu_usage),
                 validate_fp_value(ul_data_df_metrics.message_unpacking_max_latency_us),
                 validate_fp_value(ul_data_df_metrics.message_unpacking_avg_latency_us));

  const auto& dl_cp_df_metrics = cell_metrics.tx_metrics.dl_metrics.dl_cp_metrics;
  const auto& dl_up_df_metrics = cell_metrics.tx_metrics.dl_metrics.dl_up_metrics;
  const auto& ul_cp_df_metrics = cell_metrics.tx_metrics.ul_metrics.ul_cp_metrics;
  const auto& msg_tx_metrics   = cell_metrics.tx_metrics.message_tx_metrics;

  float dl_cp_cpu_usage  = dl_cp_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;
  float dl_up_cpu_usage  = dl_up_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;
  float ul_cp_cpu_usage  = ul_cp_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;
  float msg_tx_cpu_usage = msg_tx_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;

  fmt::format_to(std::back_inserter(buffer),
                 "{} cpu_usage={:.1f}% dl_up_max_latency={:.2f}us dl_up_avg_latency={:.2f}us; ",
                 "tx_dl_up:",
                 validate_fp_value(dl_up_cpu_usage),
                 validate_fp_value(dl_up_df_metrics.message_packing_max_latency_us),
                 validate_fp_value(dl_up_df_metrics.message_packing_avg_latency_us));

  fmt::format_to(std::back_inserter(buffer),
                 "{} cpu_usage={:.1f}% dl_cp_max_latency={:.2f}us dl_cp_avg_latency={:.2f}us; ",
                 "tx_dl_cp:",
                 validate_fp_value(dl_cp_cpu_usage),
                 validate_fp_value(dl_cp_df_metrics.message_packing_max_latency_us),
                 validate_fp_value(dl_cp_df_metrics.message_packing_avg_latency_us));

  fmt::format_to(std::back_inserter(buffer),
                 "{} cpu_usage={:.1f}% ul_cp_max_latency={:.2f}us ul_cp_avg_latency={:.2f}us; ",
                 "tx_ul_cp:",
                 validate_fp_value(ul_cp_cpu_usage),
                 validate_fp_value(ul_cp_df_metrics.message_packing_max_latency_us),
                 validate_fp_value(ul_cp_df_metrics.message_packing_avg_latency_us));

  fmt::format_to(std::back_inserter(buffer),
                 "{} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us; ",
                 "message_tx:",
                 validate_fp_value(msg_tx_cpu_usage),
                 validate_fp_value(msg_tx_metrics.message_tx_max_latency_us),
                 validate_fp_value(msg_tx_metrics.message_tx_avg_latency_us));

  fmt::format_to(std::back_inserter(buffer),
                 "{} nof_late_dl_grids={} nof_late_ul_req={}",
                 "tx_kpis:",
                 cell_metrics.tx_metrics.dl_metrics.nof_late_dl_grids,
                 cell_metrics.tx_metrics.ul_metrics.nof_late_ul_requests);
}

static void log_ru_ofh_metrics(srslog::log_channel& log_chan,
                               const ofh::metrics&  metrics,
                               span<const pci_t>    pci_sector_map,
                               bool                 verbose)
{
  for (const auto& cell_metrics : metrics.sectors) {
    fmt::basic_memory_buffer<char, str_buffer_size> buffer;

    const ofh::received_messages_metrics& rx_ofh_metrics = cell_metrics.rx_metrics.rx_messages_metrics;

    fmt::format_to(std::back_inserter(buffer),
                   "OFH sector#{} pci={} received messages stats: rx_total={} rx_early={} "
                   "rx_on_time={} rx_late={}",
                   cell_metrics.sector_id,
                   static_cast<unsigned>(pci_sector_map[cell_metrics.sector_id]),
                   rx_ofh_metrics.nof_early_messages + rx_ofh_metrics.nof_on_time_messages +
                       rx_ofh_metrics.nof_late_messages,
                   rx_ofh_metrics.nof_early_messages,
                   rx_ofh_metrics.nof_on_time_messages,
                   rx_ofh_metrics.nof_late_messages);

    if (verbose) {
      log_ru_ofh_performance_metrics_verbose(buffer, cell_metrics, pci_sector_map);
    }

    // Flush buffer to the logger.
    log_chan("{}", to_c_str(buffer));
  }
}

void ru_metrics_consumer_log::handle_metric(const ru_metrics& metric)
{
  if (auto* ofh_metrics = std::get_if<ofh::metrics>(&metric.metrics)) {
    log_ru_ofh_metrics(log_chan, *ofh_metrics, pci_sector_map, verbose);
  }
}

static void print_sdr_header()
{
  fmt::println("     | ------------------- TX ------------------ | ------------------- RX ------------------ |");
  fmt::println(" pci | Avg. power | Peak power | PAPR | Clipping | Avg. power | Peak power | PAPR | Clipping |");
}

void ru_metrics_handler_stdout::handle_metric(const ru_metrics& metric)
{
  if (auto* sdr_metrics = std::get_if<ru_generic_metrics>(&metric.metrics)) {
    log_ru_sdr_metrics_in_stdout(*sdr_metrics);
  }

  if (auto* dummy_metrics = std::get_if<ru_dummy_metrics>(&metric.metrics)) {
    log_ru_dummy_metrics_in_stdout(*dummy_metrics);
  }

  if (auto* ofh_metrics = std::get_if<ofh::metrics>(&metric.metrics)) {
    log_ru_ofh_metrics_in_stdout(*ofh_metrics);
  }
}

void ru_metrics_handler_stdout::log_ru_sdr_metrics_in_stdout(const ru_generic_metrics& sdr_metrics)
{
  if (sdr_metrics.cells.size() > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER) {
    print_sdr_header();
  } else if (++nof_lines > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER && !sdr_metrics.cells.empty()) {
    nof_lines = 0;
    print_sdr_header();
  }

  for (const auto& cell : sdr_metrics.cells) {
    srsran_assert(cell.sector_id < pci_sector_map.size(),
                  "Invalid sector index '{}', number of cells '{}'",
                  cell.sector_id,
                  pci_sector_map.size());

    fmt::println(" {:>3} | {:>10.1f} | {:>10.1f} | {:>4.1f} | {:>8.1e} | {:>10.1f} | {:>10.1f} | {:>4.1f} | {:>8.1e} |",
                 static_cast<unsigned>(pci_sector_map[cell.sector_id]),
                 validate_fp_value(cell.tx_avg_power_dB),
                 validate_fp_value(cell.tx_peak_power_dB),
                 validate_fp_value(cell.tx_papr_dB),
                 validate_fp_value(cell.tx_clipping_prob),
                 validate_fp_value(cell.rx_avg_power_dB),
                 validate_fp_value(cell.rx_peak_power_dB),
                 validate_fp_value(cell.rx_papr_dB),
                 validate_fp_value(cell.rx_clipping_prob));
  }
}

static void print_dummy_header()
{
  fmt::println(" pci | {:^11} | {:^11} | {:^11} | {:^11} | {:^11} | {:^11} |",
               "DL Count",
               "DL Late",
               "UL Count",
               "UL Late",
               "PRACH Count",
               "PRACH Late");
}

void ru_metrics_handler_stdout::log_ru_dummy_metrics_in_stdout(const ru_dummy_metrics& dummy_metrics)
{
  if (dummy_metrics.sectors.size() > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER) {
    print_dummy_header();
  } else if (++nof_lines > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER && !dummy_metrics.sectors.empty()) {
    nof_lines = 0;
    print_dummy_header();
  }

  for (const auto& cell : dummy_metrics.sectors) {
    srsran_assert(cell.sector_id < pci_sector_map.size(),
                  "Invalid sector index '{}', number of cells '{}'",
                  cell.sector_id,
                  pci_sector_map.size());

    fmt::println(" {:^3} | {:^11} | {:^11} | {:^11} | {:^11} | {:^11} | {:^11} |",
                 static_cast<unsigned>(pci_sector_map[cell.sector_id]),
                 cell.total_dl_request_count,
                 cell.late_dl_request_count,
                 cell.total_ul_request_count,
                 cell.late_ul_request_count,
                 cell.total_prach_request_count,
                 cell.late_prach_request_count);
  }
}

static void print_ofh_header()
{
  fmt::println("     | ----------- Number of received OFH messages ----------- |");
  fmt::println(
      " pci | {:^11} | {:^11} | {:^13} | {:^11} |", "Total Count", "Early count", "On time count", "Late count");
}

void ru_metrics_handler_stdout::log_ru_ofh_metrics_in_stdout(const ofh::metrics& ofh_metrics)
{
  if (ofh_metrics.sectors.size() > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER) {
    print_ofh_header();
  } else if (++nof_lines > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER && !ofh_metrics.sectors.empty()) {
    nof_lines = 0;
    print_ofh_header();
  }

  for (const auto& cell : ofh_metrics.sectors) {
    srsran_assert(cell.sector_id < pci_sector_map.size(),
                  "Invalid sector index '{}', number of cells '{}'",
                  cell.sector_id,
                  pci_sector_map.size());

    const ofh::received_messages_metrics& rx_metrics = cell.rx_metrics.rx_messages_metrics;

    fmt::println(" {:^3} | {:^11} | {:^11} | {:^13} | {:^11} |",
                 static_cast<unsigned>(pci_sector_map[cell.sector_id]),
                 rx_metrics.nof_early_messages + rx_metrics.nof_late_messages + rx_metrics.nof_on_time_messages,
                 rx_metrics.nof_early_messages,
                 rx_metrics.nof_on_time_messages,
                 rx_metrics.nof_late_messages);
  }
}
