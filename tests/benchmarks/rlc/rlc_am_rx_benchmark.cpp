/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/rlc/rlc_rx_am_entity.h"
#include "lib/rlc/rlc_tx_am_entity.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <getopt.h>

using namespace srsran;

/// Mocking class of the surrounding layers invoked by the RLC AM Tx entity.
class rlc_tx_am_test_frame : public rlc_tx_upper_layer_data_notifier,
                             public rlc_tx_upper_layer_control_notifier,
                             public rlc_tx_lower_layer_notifier,
                             public rlc_rx_am_status_provider
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
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn) override {}

  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(unsigned bsr_) override {}

  // rlc_rx_am_status_provider interface
  rlc_am_status_pdu& get_status_pdu() override { return status; }
  uint32_t           get_status_pdu_length() override { return status.get_packed_size(); }
  bool               status_report_required() override { return status_required; }
};

/// Mocking class of the surrounding layers invoked by the RLC AM Rx entity.
class rlc_rx_am_test_frame : public rlc_rx_upper_layer_data_notifier,
                             public rlc_tx_am_status_handler,
                             public rlc_tx_am_status_notifier
{
public:
  rlc_rx_am_test_frame() {}

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override {}

  // rlc_tx_am_status_handler interface
  virtual void on_status_pdu(rlc_am_status_pdu status_) override {}
  // rlc_tx_am_status_notifier interface
  virtual void on_status_report_changed() override {}
};

struct bench_params {
  unsigned nof_repetitions = 10000;
};

enum class rx_order {
  in_order,      ///< 0, 1, 2,..., N-1
  swapped_edges, ///< N-1, 1, 2,..., N-2, 0
  reverse_order, ///< N-1, N-2, N-3,..., 0
  even_odd       ///< 0, 2, 4,...,1, 3, 5,...
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:h")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0], params);
        exit(0);
    }
  }
}

std::vector<byte_buffer> generate_pdus(bench_params params, rx_order order)
{
  // Set Tx config
  rlc_tx_am_config config;
  config.sn_field_length = rlc_am_sn_size::size18bits;
  config.t_poll_retx     = 45;
  config.max_retx_thresh = 4;
  config.poll_pdu        = 4;
  config.poll_byte       = 25;
  config.queue_size      = 4096;

  // Create test frame
  auto tester = std::make_unique<rlc_tx_am_test_frame>(config.sn_field_length);

  timer_manager      timers;
  manual_task_worker pcell_worker{128};
  manual_task_worker ue_worker{128};

  // Create RLC AM TX entity
  std::unique_ptr<rlc_tx_am_entity> rlc_tx = nullptr;

  auto& logger = srslog::fetch_basic_logger("RLC");
  logger.set_level(srslog::str_to_basic_level("warning"));

  null_rlc_pcap pcap;

  // Make PDUs
  std::vector<byte_buffer> pdus;
  rlc_tx = std::make_unique<rlc_tx_am_entity>(0,
                                              du_ue_index_t::MIN_DU_UE_INDEX,
                                              srb_id_t::srb0,
                                              config,
                                              *tester,
                                              *tester,
                                              *tester,
                                              timer_factory{timers, pcell_worker},
                                              pcell_worker,
                                              ue_worker,
                                              false,
                                              pcap);

  // Bind AM Rx/Tx interconnect
  rlc_tx->set_status_provider(tester.get());

  // Prepare SDU list for benchmark
  std::vector<byte_buffer> sdu_list  = {};
  int                      num_sdus  = params.nof_repetitions + 1; // +1 to expire t_reassembly on setup
  int                      num_bytes = 1500;
  for (int i = 0; i < num_sdus; i++) {
    byte_buffer sdu_buf = {};
    for (int j = 0; j < num_bytes; ++j) {
      report_error_if_not(sdu_buf.append(rand()), "Failed to allocate SDU");
    }
    sdu_list.push_back(std::move(sdu_buf));
  }

  for (int i = 0; i < num_sdus; i++) {
    rlc_sdu     sdu;
    byte_buffer pdcp_hdr_buf = {0x80, 0x00, 0x16};
    byte_buffer sdu_buf      = std::move(sdu_list[i]);
    sdu.pdcp_sn              = i;
    sdu.buf                  = std::move(pdcp_hdr_buf);
    report_error_if_not(sdu.buf.append(std::move(sdu_buf)), "Failed to allocate SDU");
    rlc_tx->handle_sdu(std::move(sdu));
    std::vector<uint8_t> pdu_buf;
    pdu_buf.resize(1550);
    size_t pdu_len = rlc_tx->pull_pdu(pdu_buf);
    pdu_buf.resize(pdu_len);
    pdus.push_back(byte_buffer{pdu_buf});
  }

  // shuffle PDUs according to requested order
  switch (order) {
    case rx_order::in_order:
      break;
    case rx_order::swapped_edges:
      std::swap(pdus.front(), pdus.back());
      break;
    case rx_order::reverse_order:
      std::reverse(pdus.begin(), pdus.end());
      break;
    case rx_order::even_odd:
      std::vector<byte_buffer> sdu_list_mod;
      for (int i = 0; i < num_sdus; i += 2) {
        sdu_list_mod.push_back(std::move(sdu_list[i]));
      }
      for (int i = 1; i < num_sdus; i += 2) {
        sdu_list_mod.push_back(std::move(sdu_list[i]));
      }
      sdu_list = std::move(sdu_list_mod);
      break;
  }

  timers.tick();
  return pdus;
}

void benchmark_rx_pdu(const bench_params& params, rx_order order)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "Benchmark RLC AM RX PDUs ({})", order);
  std::unique_ptr<benchmarker> bm = std::make_unique<benchmarker>(to_c_str(buffer), params.nof_repetitions);

  auto tester = std::make_unique<rlc_rx_am_test_frame>();

  timer_manager      timers;
  manual_task_worker pcell_worker{128};
  manual_task_worker ue_worker{128};

  null_rlc_pcap pcap;

  // Set Tx config
  rlc_rx_am_config config;
  config.sn_field_length   = rlc_am_sn_size::size18bits;
  config.t_status_prohibit = 0;
  config.t_reassembly      = 200;

  // Create RLC AM RX entity
  std::unique_ptr<rlc_rx_am_entity> rlc_rx = std::make_unique<rlc_rx_am_entity>(0,
                                                                                du_ue_index_t::MIN_DU_UE_INDEX,
                                                                                srb_id_t::srb0,
                                                                                config,
                                                                                *tester,
                                                                                timer_factory{timers, ue_worker},
                                                                                ue_worker,
                                                                                false,
                                                                                pcap);

  // Bind AM Rx/Tx interconnect
  rlc_rx->set_status_notifier(tester.get());

  std::vector<byte_buffer> pdus = generate_pdus(params, order);

  unsigned i = 0;

  // Push first PDU and expire reassembly timer to advance rx_highest_status
  rlc_rx->handle_pdu(std::move(pdus[i++]));
  for (int32_t t = 0; t < config.t_reassembly + 1; t++) {
    timers.tick();
  }
  ue_worker.run_pending_tasks();

  auto measure = [&rlc_rx, &i, &pdus]() mutable {
    rlc_rx->handle_pdu(std::move(pdus[i]));
    i++;
  };
  bm->new_measure("RX RLC AM PDU", 1500 * 8, measure);

  // Output results.
  bm->print_percentiles_time();
  bm->print_percentiles_throughput(" bps");
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::error);

  srslog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  // Setup size of byte buffer pool.
  init_byte_buffer_segment_pool(524288);

  {
    benchmark_rx_pdu(params, rx_order::in_order);
  }
  {
    benchmark_rx_pdu(params, rx_order::swapped_edges);
  }
  {
    benchmark_rx_pdu(params, rx_order::reverse_order);
  }
  {
    benchmark_rx_pdu(params, rx_order::even_odd);
  }
  srslog::flush();
}

//
// Formatters
//
namespace fmt {

// RLC mode
template <>
struct formatter<rx_order> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(rx_order order, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"in order", "swapped edges", "reverse order", "even odd"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(order)]);
  }
};

} // namespace fmt
