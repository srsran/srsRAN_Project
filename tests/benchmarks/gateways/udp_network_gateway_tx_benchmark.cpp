/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "udp_network_gateway_benchmark_helpers.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <queue>

using namespace srsran;

struct bench_params {
  unsigned pdu_len  = 1400;
  unsigned nof_pdus = 100000;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-n <nof PDUs>] [-l <PDU len>] [-u <t us>]\n", prog);
  fmt::print("\t-l PDU len [Default {}]\n", params.pdu_len);
  fmt::print("\t-n Number of PDUs [Default {}]\n", params.nof_pdus);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "l:n:u:h")) != -1) {
    switch (opt) {
      case 'l':
        params.pdu_len = std::strtol(optarg, nullptr, 10);
        break;
      case 'n':
        params.nof_pdus = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0], params);
        exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  srslog::init();

  // init GW logger
  srslog::fetch_basic_logger("UDP-GW", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UDP-GW", true).set_hex_dump_max_size(100);

  bench_params params{};
  parse_args(argc, argv, params);

  udp_network_gateway_config gw1_cfg;
  gw1_cfg.bind_address      = "127.0.0.1";
  gw1_cfg.bind_port         = 56701;
  gw1_cfg.non_blocking_mode = false;
  gw1_cfg.rx_max_mmsg       = 256;

  dummy_network_gateway_data_notifier_with_src_addr gw1_dn{0}; // no rx required
  std::unique_ptr<udp_network_gateway>              gw1;
  std::unique_ptr<udp_network_gateway>              gw2;

  manual_task_worker io_tx_executor{128};

  gw1 = create_udp_network_gateway({gw1_cfg, gw1_dn, io_tx_executor});

  gw1->create_and_bind();

  sockaddr_storage gw2_addr = to_sockaddr_storage("127.0.0.1", 56702);

  byte_buffer pdu     = make_tx_byte_buffer(params.pdu_len);
  uint32_t    pdu_len = pdu.length();

  auto t_start = std::chrono::high_resolution_clock::now();

  unsigned nof_pdus = params.nof_pdus;
  for (unsigned n = 0; n < nof_pdus; n++) {
    gw1->handle_pdu(pdu.copy(), gw2_addr);
  }
  auto t_end    = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
  fmt::print("Tx done\n\n");

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  fmt::print("Tx time: {} us\n", duration.count());
  fmt::print("Tx data rate: {:.2f} Mbit/s\n", (double)pdu_len * nof_pdus * 8 * 1e-6 / (duration.count() * 1e-6));
  fmt::print("Tx PDU rate: {:.2f} PDU/s\n", (double)nof_pdus / (duration.count() * 1e-6));
  fmt::print("Tx PDUs total: {:>7}\n", nof_pdus);
}
