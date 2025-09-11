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

#include "lib/rlc/rlc_tx_am_entity.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <getopt.h>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC AM Tx entity.
class rlc_tx_am_test_frame : public rlc_tx_upper_layer_data_notifier,
                             public rlc_tx_upper_layer_control_notifier,
                             public rlc_tx_lower_layer_notifier,
                             public rlc_rx_am_status_provider,
                             public rlc_metrics_notifier
{
public:
  rlc_am_sn_size    sn_size;
  rlc_am_status_pdu status;
  bool              status_required = false;
  uint32_t          bsr             = 0;
  uint32_t          bsr_count       = 0;
  uint32_t          max_retx_count  = 0;

  rlc_tx_am_test_frame(rlc_am_sn_size sn_size_) : sn_size(sn_size_), status(sn_size_) {}

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn, uint32_t desired_buf_size) override {}
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}
  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override {}
  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(const rlc_buffer_state& bs) override {}

  // rlc_rx_am_status_provider interface
  rlc_am_status_pdu& get_status_pdu() override { return status; }
  uint32_t           get_status_pdu_length() override { return status.get_packed_size(); }
  bool               status_report_required() override { return status_required; }

  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

struct bench_params {
  unsigned nof_repetitions = 10000;
  unsigned sdu_size        = 1500;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-s SDU size [Default {}]\n", params.sdu_size);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:s:p:h")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 's':
        params.sdu_size = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0], params);
        std::exit(0);
    }
  }
}

std::vector<std::vector<uint8_t>>
push_sdus(bench_params params, const rlc_tx_am_config& config, rlc_tx_am_entity* rlc_tx)
{
  timer_manager      timers;
  manual_task_worker pcell_worker{128};
  manual_task_worker ue_worker{128};

  // Make PDUs
  std::vector<std::vector<uint8_t>> pdus;

  // Prepare SDU list for benchmark
  int num_sdus  = params.nof_repetitions;
  int num_bytes = params.sdu_size;
  pdus.resize(num_sdus);
  for (int i = 0; i < num_sdus; i++) {
    byte_buffer sdu = test_helpers::create_pdcp_pdu(config.pdcp_sn_len, /* is_srb = */ false, i, num_bytes, i);
    rlc_tx->handle_sdu(std::move(sdu), false);
    pdus[i].resize(params.sdu_size + 5); // reserve max possible header
  }

  return pdus;
}

void benchmark_tx_pdu(const bench_params& params)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "Benchmark RLC AM TX PDUs");
  std::unique_ptr<benchmarker> bm = std::make_unique<benchmarker>(to_c_str(buffer), params.nof_repetitions);

  // Set Tx config
  rlc_tx_am_config config;
  config.sn_field_length  = rlc_am_sn_size::size18bits;
  config.pdcp_sn_len      = pdcp_sn_size::size18bits;
  config.t_poll_retx      = 45;
  config.max_retx_thresh  = 4;
  config.poll_pdu         = 4;
  config.poll_byte        = 25;
  config.queue_size       = params.nof_repetitions;
  config.queue_size_bytes = params.nof_repetitions * 1507;
  config.max_window       = 0;

  auto tester = std::make_unique<rlc_tx_am_test_frame>(config.sn_field_length);

  timer_manager      timers;
  manual_task_worker pcell_worker{128};
  manual_task_worker ue_worker{128};

  null_rlc_pcap pcap;

  auto metrics_coll = std::make_unique<rlc_bearer_metrics_collector>(
      gnb_du_id_t{}, du_ue_index_t{}, rb_id_t{}, timer_duration{1}, tester.get(), ue_worker);

  // Create RLC AM TX entity
  auto rlc_tx = std::make_unique<rlc_tx_am_entity>(gnb_du_id_t::min,
                                                   du_ue_index_t::MIN_DU_UE_INDEX,
                                                   drb_id_t::drb1,
                                                   config,
                                                   *tester,
                                                   *tester,
                                                   *tester,
                                                   *metrics_coll,
                                                   pcap,
                                                   pcell_worker,
                                                   ue_worker,
                                                   timers);

  // Bind AM Rx/Tx interconnect
  rlc_tx->set_status_provider(tester.get());

  std::vector<std::vector<uint8_t>> pdus = push_sdus(params, config, rlc_tx.get());

  unsigned i = 0;

  // Push first PDU and expire reassembly timer to advance rx_highest_status
  auto measure = [&rlc_tx, &i, &pdus]() {
    rlc_tx->pull_pdu(pdus[i]);
    i++;
  };
  bm->new_measure("TX RLC AM PDU", params.sdu_size * 8, measure);

  // Output results.
  bm->print_percentiles_time();
  bm->print_percentiles_throughput(" bps");

  uint64_t                 bm_duration_ns = bm->get_total_meas_time_ns();
  std::chrono::nanoseconds bm_duration(bm_duration_ns);
  rlc_tx_metrics           tx_metrics = rlc_tx->get_metrics();
  fmt::print("\nRLC TX metrics:\n");
  buffer.clear();
  format_rlc_tx_metrics(buffer, std::chrono::duration_cast<std::chrono::milliseconds>(bm_duration), tx_metrics);
  fmt::print(" - {}\n", to_c_str(buffer));
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::warning);

  srslog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  // Setup size of byte buffer pool.
  init_byte_buffer_segment_pool(524288);

  benchmark_tx_pdu(params);

  srslog::flush();
}
