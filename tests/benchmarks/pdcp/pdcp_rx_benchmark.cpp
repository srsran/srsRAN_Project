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

#include "lib/pdcp/pdcp_entity_rx.h"
#include "lib/pdcp/pdcp_entity_tx.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <getopt.h>

using namespace srsran;

static constexpr std::array<uint8_t, 16> k_128_int =
    {0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
static constexpr std::array<uint8_t, 16> k_128_enc =
    {0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

namespace {

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_tx_gen_frame : public pdcp_tx_lower_notifier, public pdcp_tx_upper_control_notifier
{
public:
  /// PDCP TX upper layer control notifier
  void on_max_count_reached() final {}
  void on_protocol_failure() final {}

  /// PDCP TX lower layer data notifier
  void on_new_pdu(pdcp_tx_pdu pdu) final { pdu_list.push_back(byte_buffer_chain::create(std::move(pdu.buf)).value()); }
  void on_discard_pdu(uint32_t pdcp_sn) final {}
  std::vector<byte_buffer_chain> pdu_list;
};

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_rx_test_frame : public pdcp_tx_status_handler,
                           public pdcp_rx_upper_data_notifier,
                           public pdcp_rx_upper_control_notifier
{
public:
  /// PDCP TX status handler
  void on_status_report(byte_buffer_chain status) override {}

  /// PDCP RX upper layer data notifier
  void on_new_sdu(byte_buffer sdu) override {}

  /// PDCP RX upper layer control notifier
  void on_integrity_failure() override {}
  void on_protocol_failure() override {}
  void on_max_count_reached() override {}
};

struct bench_params {
  unsigned nof_repetitions   = 1000;
  bool     print_timing_info = false;
};

struct app_params {
  int         algo         = -1;
  std::string log_level    = "error";
  std::string log_filename = "stdout";
};

} // namespace

static void usage(const char* prog, const bench_params& params, const app_params& app)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-a Security algorithm to use [Default {}, valid {{-1,0,1,2,3}}]\n", app.algo);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-l Log level to use [Default {}, valid {{error, warning, info, debug}}]\n", app.log_level);
  fmt::print("\t-f Log filename to use [Default {}]\n", app.log_filename);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params, app_params& app)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "a:R:l:f:th")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'a':
        app.algo = std::strtol(optarg, nullptr, 10);
        break;
      case 't':
        params.print_timing_info = true;
        break;
      case 'l':
        app.log_level = std::string(optarg);
        break;
      case 'f':
        app.log_filename = std::string(optarg);
        break;
      case 'h':
      default:
        usage(argv[0], params, app);
        exit(0);
    }
  }
}

std::vector<byte_buffer_chain> gen_pdu_list(bench_params                  params,
                                            security::integrity_enabled   int_enabled,
                                            security::ciphering_enabled   ciph_enabled,
                                            security::integrity_algorithm int_algo,
                                            security::ciphering_algorithm ciph_algo)
{
  timer_manager      timers;
  manual_task_worker worker{64};

  // Set TX config
  pdcp_tx_config config         = {};
  config.rb_type                = pdcp_rb_type::drb;
  config.rlc_mode               = pdcp_rlc_mode::am;
  config.sn_size                = pdcp_sn_size::size18bits;
  config.direction              = pdcp_security_direction::uplink;
  config.discard_timer          = pdcp_discard_timer::ms10;
  config.status_report_required = false;

  security::sec_128_as_config sec_cfg = {};

  // Set security domain
  sec_cfg.domain = security::sec_domain::up; // DRB

  // Set security keys
  sec_cfg.k_128_int = k_128_int;
  sec_cfg.k_128_enc = k_128_enc;

  // Set encryption/integrity algorithms
  sec_cfg.integ_algo  = int_algo;
  sec_cfg.cipher_algo = ciph_algo;

  // Create test frame
  pdcp_tx_gen_frame frame = {};

  // Create PDCP entities
  std::unique_ptr<pdcp_entity_tx> pdcp_tx =
      std::make_unique<pdcp_entity_tx>(0, drb_id_t::drb1, config, frame, frame, timer_factory{timers, worker});
  pdcp_tx->configure_security(sec_cfg);
  pdcp_tx->set_integrity_protection(int_enabled);
  pdcp_tx->set_ciphering(ciph_enabled);

  // Prepare SDU list for benchmark
  int num_sdus  = params.nof_repetitions;
  int num_bytes = 1500;
  for (int i = 0; i < num_sdus; i++) {
    byte_buffer sdu_buf = {};
    for (int j = 0; j < num_bytes; ++j) {
      report_error_if_not(sdu_buf.append(rand()), "Failed to allocate SDU");
    }
    pdcp_tx->handle_sdu(std::move(sdu_buf));
  }
  return std::move(frame.pdu_list);
}

void benchmark_pdcp_rx(bench_params                  params,
                       security::integrity_enabled   int_enabled,
                       security::ciphering_enabled   ciph_enabled,
                       security::integrity_algorithm int_algo,
                       security::ciphering_algorithm ciph_algo)
{
  fmt::memory_buffer buffer;
  if (int_enabled == security::integrity_enabled::on || ciph_enabled == security::ciphering_enabled::on) {
    fmt::format_to(buffer, "Benchmark PDCP RX. NIA{} NEA{}", int_algo, ciph_algo);
  } else {
    fmt::format_to(buffer, "Benchmark PDCP RX. NIA0 NEA0");
  }

  std::vector<byte_buffer_chain> pdu_list = gen_pdu_list(params, int_enabled, ciph_enabled, int_algo, ciph_algo);

  std::unique_ptr<benchmarker> bm = std::make_unique<benchmarker>(to_c_str(buffer), params.nof_repetitions);

  timer_manager      timers;
  manual_task_worker worker{64};

  // Set TX config
  pdcp_rx_config config = {};
  config.rb_type        = pdcp_rb_type::drb;
  config.rlc_mode       = pdcp_rlc_mode::am;
  config.sn_size        = pdcp_sn_size::size18bits;
  config.direction      = pdcp_security_direction::downlink;
  config.t_reordering   = pdcp_t_reordering::ms100;

  security::sec_128_as_config sec_cfg = {};

  // Set security domain
  sec_cfg.domain = security::sec_domain::up; // DRB

  // Set security keys
  sec_cfg.k_128_int = k_128_int;
  sec_cfg.k_128_enc = k_128_enc;

  // Set encryption/integrity algorithms
  sec_cfg.integ_algo  = int_algo;
  sec_cfg.cipher_algo = ciph_algo;

  // Create test frame
  pdcp_rx_test_frame frame = {};

  // Create PDCP entities
  std::unique_ptr<pdcp_entity_rx> pdcp_rx =
      std::make_unique<pdcp_entity_rx>(0, drb_id_t::drb1, config, frame, frame, timer_factory{timers, worker});
  pdcp_rx->configure_security(sec_cfg);
  pdcp_rx->set_integrity_protection(int_enabled);
  pdcp_rx->set_ciphering(ciph_enabled);

  // Prepare SDU list for benchmark
  std::vector<byte_buffer> sdu_list  = {};
  int                      num_sdus  = params.nof_repetitions;
  int                      num_bytes = 1500;
  for (int i = 0; i < num_sdus; i++) {
    byte_buffer sdu_buf = {};
    for (int j = 0; j < num_bytes; ++j) {
      report_error_if_not(sdu_buf.append(rand()), "Failed to allocate SDU");
    }
    sdu_list.push_back(std::move(sdu_buf));
  }

  // Run benchmark.
  int  pdcp_sn = 0;
  auto measure = [&pdcp_rx, pdcp_sn, &pdu_list]() mutable {
    pdcp_rx->handle_pdu(std::move(pdu_list[pdcp_sn]));
    pdcp_sn++;
  };
  bm->new_measure("RX PDU", 1500 * 8, measure);

  // Output results.
  if (params.print_timing_info) {
    bm->print_percentiles_time();
  }
  bm->print_percentiles_throughput(" bps");
}

int run_benchmark(bench_params params, int algo)
{
  if (algo != 0 && algo != 1 && algo != 2 && algo != 3) {
    fmt::print("Unsupported algorithm. Use NIA/NEA 0, 1, 2 or 3.\n");
    return -1;
  }
  fmt::print("------ Benchmarking: NIA{} NEA{} ------\n", algo, algo);

  security::integrity_algorithm int_algo  = static_cast<security::integrity_algorithm>(algo);
  security::ciphering_algorithm ciph_algo = static_cast<security::ciphering_algorithm>(algo);

  if (algo == 0) {
    benchmark_pdcp_rx(params,
                      security::integrity_enabled::off,
                      security::ciphering_enabled::off,
                      security::integrity_algorithm::nia2, // NIA0 is forbidden, use NIA2 and disable integrity
                      ciph_algo);
  } else {
    benchmark_pdcp_rx(params, security::integrity_enabled::on, security::ciphering_enabled::on, int_algo, ciph_algo);
    benchmark_pdcp_rx(params, security::integrity_enabled::on, security::ciphering_enabled::off, int_algo, ciph_algo);
    benchmark_pdcp_rx(params, security::integrity_enabled::off, security::ciphering_enabled::on, int_algo, ciph_algo);
  }
  fmt::print("------ End of Benchmark ------\n");
  return 0;
}

int main(int argc, char** argv)
{
  bench_params params{};
  app_params   app_params{};
  parse_args(argc, argv, params, app_params);

  srslog::init();
  srslog::sink* log_sink = (app_params.log_filename == "stdout") ? srslog::create_stdout_sink()
                                                                 : srslog::create_file_sink(app_params.log_filename);
  if (log_sink == nullptr) {
    return -1;
  }
  srslog::set_default_sink(*log_sink);
  srslog::fetch_basic_logger("PDCP").set_level(srslog::str_to_basic_level(app_params.log_level));

  if (app_params.algo != -1 && app_params.algo != 0 && app_params.algo != 1 && app_params.algo != 2 &&
      app_params.algo != 3) {
    fmt::print("Unsupported algorithm. Use -1, 0, 1, 2 or 3.\n");
    return -1;
  }

  if (app_params.algo != -1) {
    run_benchmark(params, app_params.algo);
  } else {
    for (unsigned i = 0; i < 4; i++) {
      run_benchmark(params, i);
    }
  }
  srslog::flush();
}
