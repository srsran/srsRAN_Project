/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
  rlc_am_status_pdu get_status_pdu() override { return status; }
  uint32_t          get_status_pdu_length() override { return status.get_packed_size(); }
  bool              status_report_required() override { return status_required; }
};

struct bench_params {
  unsigned nof_repetitions = 100;
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

void benchmark_status_pdu_handling(rlc_am_status_pdu status)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "Benchmark status report handling. {}", status);
  std::unique_ptr<benchmarker> bm = std::make_unique<benchmarker>(to_c_str(buffer), 100);

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
  std::unique_ptr<rlc_tx_am_entity> rlc = nullptr;

  auto& logger = srslog::fetch_basic_logger("RLC");
  logger.set_level(srslog::str_to_basic_level("warning"));

  pcap_rlc_dummy pcap;

  // Run benchmark
  auto context = [&rlc, &tester, config, &timers, &pcell_worker, &ue_worker, &pcap]() {
    rlc = std::make_unique<rlc_tx_am_entity>(du_ue_index_t::MIN_DU_UE_INDEX,
                                             srb_id_t::srb0,
                                             config,
                                             *tester,
                                             *tester,
                                             *tester,
                                             timer_factory{timers, pcell_worker},
                                             pcell_worker,
                                             ue_worker,
                                             pcap);

    // Bind AM Rx/Tx interconnect
    rlc->set_status_provider(tester.get());

    for (int i = 0; i < 1000; i++) {
      rlc_sdu     sdu;
      byte_buffer pdcp_hdr_buf = {0x80, 0x00, 0x16};
      byte_buffer sdu_buf      = {0x00, 0x01, 0x02, 0x04};
      sdu.pdcp_sn              = i;
      sdu.buf                  = std::move(pdcp_hdr_buf);
      sdu.buf.append(std::move(sdu_buf));
      rlc->handle_sdu(std::move(sdu));
      rlc->pull_pdu(100);
    }
  };
  auto measure = [&rlc, &status]() mutable { rlc->on_status_pdu(status); };
  bm->new_measure_with_context("Handling status pdu", 1, context, measure);

  // Output results.
  bm->print_percentiles_time();
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::error);

  srslog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  {
    rlc_am_status_pdu status(rlc_am_sn_size::size18bits);
    status.ack_sn = 1000;
    benchmark_status_pdu_handling(status);
  }
  {
    rlc_am_status_pdu status(rlc_am_sn_size::size18bits);
    status.ack_sn           = 1000;
    rlc_am_status_nack nack = {}; // NACK SN=0
    status.push_nack(nack);
    benchmark_status_pdu_handling(status);
  }
  srslog::flush();
}
