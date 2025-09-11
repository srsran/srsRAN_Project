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
#include "apps/helpers/metrics/helpers.h"
#include "apps/helpers/metrics/json_generators/ru/ofh.h"
#include "srsran/ru/ru_metrics.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;
using namespace app_helpers;

/// Size in bytes of the buffer used to prepare the metrics before logging.
static constexpr unsigned str_buffer_size = 2048;

static void log_ru_ofh_metrics_json(srslog::log_channel&     log_chan,
                                    const ofh::metrics&      metrics,
                                    span<const pci_t>        pci_sector_map,
                                    std::chrono::nanoseconds symbol_duration)
{
  log_chan("{}", app_helpers::json_generators::generate_string(metrics, pci_sector_map, symbol_duration, 2));
}

void ru_metrics_consumer_json::handle_metric(const ru_metrics& metric)
{
  if (const auto* ofh_metrics = std::get_if<ofh::metrics>(&metric.metrics)) {
    log_ru_ofh_metrics_json(log_chan, *ofh_metrics, pci_sector_map, symbol_duration);
  }
}

static void log_ru_ofh_performance_metrics_verbose(fmt::basic_memory_buffer<char, str_buffer_size>& buffer,
                                                   const ofh::sector_metrics&                       cell_metrics)
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
                 "{} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us throughput={:.2f}Mbps "
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

  // Message decoder.
  const auto& decoder_metrics = cell_metrics.rx_metrics.rx_decoding_perf_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{} nof_past_seqid_msg={} nof_future_seqid_msg={}; ",
                 "ecpri:",
                 decoder_metrics.nof_dropped_messages,
                 decoder_metrics.nof_skipped_messages);

  const auto& ul_prach_df_metrics = decoder_metrics.prach_processing_metrics;
  const auto& ul_data_df_metrics  = decoder_metrics.data_processing_metrics;

  float prach_unpacking_cpu_usage =
      ul_prach_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;
  float data_unpacking_cpu_usage =
      ul_data_df_metrics.cpu_usage_us / (cell_metrics.metrics_period_ms.count() * 1e3) * 100.0f;

  fmt::format_to(std::back_inserter(buffer),
                 "{} nof_dropped_msg={} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us; ",
                 "rcv_prach:",
                 ul_prach_df_metrics.nof_dropped_messages,
                 validate_fp_value(prach_unpacking_cpu_usage),
                 validate_fp_value(ul_prach_df_metrics.message_unpacking_max_latency_us),
                 validate_fp_value(ul_prach_df_metrics.message_unpacking_avg_latency_us));
  fmt::format_to(std::back_inserter(buffer),
                 "{} nof_dropped_msg={} cpu_usage={:.1f}% max_latency={:.2f}us avg_latency={:.2f}us; ",
                 "rcv_ul:",
                 ul_data_df_metrics.nof_dropped_messages,
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
                 "{} nof_late_dl_rgs={} nof_late_ul_req={} nof_late_cp_dl={} nof_late_up_dl={} nof_late_cp_ul={}",
                 "tx_kpis:",
                 cell_metrics.tx_metrics.dl_metrics.nof_late_dl_grids,
                 cell_metrics.tx_metrics.ul_metrics.nof_late_ul_requests,
                 cell_metrics.tx_metrics.dl_metrics.nof_late_cp_dl,
                 cell_metrics.tx_metrics.dl_metrics.nof_late_up_dl,
                 cell_metrics.tx_metrics.ul_metrics.nof_late_cp_ul);
}

static void log_ru_ofh_metrics(srslog::log_channel&     log_chan,
                               const ofh::metrics&      metrics,
                               span<const pci_t>        pci_sector_map,
                               bool                     verbose,
                               std::chrono::nanoseconds symbol_duration)
{
  fmt::basic_memory_buffer<char, str_buffer_size> buffer;

  fmt::format_to(std::back_inserter(buffer),
                 "OFH metrics: timing metrics: nof_skipped_symbols={} skipped_symbols_max_burst={}; ",
                 metrics.timing.nof_skipped_symbols,
                 metrics.timing.skipped_symbols_max_burst);

  for (const auto& cell_metrics : metrics.sectors) {
    const ofh::received_messages_metrics& rx_ofh_metrics    = cell_metrics.rx_metrics.rx_messages_metrics;
    const ofh::closed_rx_window_metrics&  rx_closed_metrics = cell_metrics.rx_metrics.closed_window_metrics;

    fmt::format_to(
        std::back_inserter(buffer),
        "sector#{} pci={} received messages stats: rx_total={} rx_early={} "
        "rx_on_time={} rx_late={} earliest_msg_us={:.2f} latest_msg_us={:.2f}, nof_missed_uplink_symbols={} "
        "nof_missed_prach_occasions={}; ",
        cell_metrics.sector_id,
        static_cast<unsigned>(pci_sector_map[cell_metrics.sector_id]),
        rx_ofh_metrics.nof_early_messages + rx_ofh_metrics.nof_on_time_messages + rx_ofh_metrics.nof_late_messages,
        rx_ofh_metrics.nof_early_messages,
        rx_ofh_metrics.nof_on_time_messages,
        rx_ofh_metrics.nof_late_messages,
        validate_fp_value(static_cast<float>(rx_ofh_metrics.earliest_rx_msg_in_symbols) * symbol_duration.count() *
                          1e-3),
        validate_fp_value(static_cast<float>(rx_ofh_metrics.latest_rx_msg_in_symbols) * symbol_duration.count() * 1e-3),
        rx_closed_metrics.nof_missing_uplink_symbols,
        rx_closed_metrics.nof_missing_prach_contexts);

    if (verbose) {
      log_ru_ofh_performance_metrics_verbose(buffer, cell_metrics);
    }
  }
  log_chan("{}", to_c_str(buffer));
}

void ru_metrics_consumer_log::handle_metric(const ru_metrics& metric)
{
  if (const auto* ofh_metrics = std::get_if<ofh::metrics>(&metric.metrics)) {
    log_ru_ofh_metrics(log_chan, *ofh_metrics, pci_sector_map, verbose, symbol_duration);
  }
}

static void print_sdr_header()
{
  fmt::println("     | ------------------- TX ------------------ | ------------------- RX ------------------ |");
  fmt::println(" pci | Avg. power | Peak power | PAPR | Clipping | Avg. power | Peak power | PAPR | Clipping |");
}

void ru_metrics_handler_stdout::handle_metric(const ru_metrics& metric)
{
  if (const auto* sdr_metrics = std::get_if<ru_generic_metrics>(&metric.metrics)) {
    log_ru_sdr_metrics_in_stdout(*sdr_metrics);
  }

  if (const auto* dummy_metrics = std::get_if<ru_dummy_metrics>(&metric.metrics)) {
    log_ru_dummy_metrics_in_stdout(*dummy_metrics);
  }

  if (const auto* ofh_metrics = std::get_if<ofh::metrics>(&metric.metrics)) {
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
  fmt::println("     | -------------------------- Number of received OFH messages ------------------------- |");
  fmt::println(" pci | {:^11} | {:^11} | {:^13} | {:^11} | {:^12} | {:^11} |",
               "Total Count",
               "Early count",
               "On time count",
               "Late count",
               "Earliest msg",
               "Latest msg");
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

    fmt::println(
        " {:^3} | {:^11} | {:^11} | {:^13} | {:^11} | {:^12} | {:^11} |",
        static_cast<unsigned>(pci_sector_map[cell.sector_id]),
        rx_metrics.nof_early_messages + rx_metrics.nof_late_messages + rx_metrics.nof_on_time_messages,
        rx_metrics.nof_early_messages,
        rx_metrics.nof_on_time_messages,
        rx_metrics.nof_late_messages,
        validate_fp_value(static_cast<float>(rx_metrics.earliest_rx_msg_in_symbols) * symbol_duration.count() * 1e-3),
        validate_fp_value(static_cast<float>(rx_metrics.latest_rx_msg_in_symbols) * symbol_duration.count() * 1e-3));
  }
}
