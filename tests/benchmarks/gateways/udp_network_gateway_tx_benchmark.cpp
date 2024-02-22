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

#include "udp_network_gateway_benchmark_helpers.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <queue>

using namespace srsran;

struct bench_params {
  uint64_t pdu_len  = 1400;
  uint64_t nof_pdus = 100000;
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
  while ((opt = getopt(argc, argv, "l:n:h")) != -1) {
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
  srslog::fetch_basic_logger("IO-EPOLL", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UDP-GW", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UDP-GW", true).set_hex_dump_max_size(100);

  bench_params params{};
  parse_args(argc, argv, params);

  udp_network_gateway_config gw1_cfg;
  gw1_cfg.bind_address      = "127.0.0.1";
  gw1_cfg.bind_port         = 56701;
  gw1_cfg.non_blocking_mode = false;
  gw1_cfg.rx_max_mmsg       = 256;

  dummy_network_gateway_data_notifier_with_src_addr gw1_dn{0, params.nof_pdus}; // no rx required
  std::unique_ptr<udp_network_gateway>              gw1;
  std::unique_ptr<udp_network_gateway>              gw2;

  manual_task_worker io_tx_executor{128};

  gw1 = create_udp_network_gateway({gw1_cfg, gw1_dn, io_tx_executor});

  gw1->create_and_bind();

  sockaddr_storage gw2_addr = to_sockaddr_storage("127.0.0.1", 56702);

  byte_buffer pdu = make_tx_byte_buffer(params.pdu_len);

  auto t_start = std::chrono::high_resolution_clock::now();

  for (unsigned n = 0; n < params.nof_pdus; n++) {
    gw1->handle_pdu(pdu.copy(), gw2_addr);
  }
  auto t_end    = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
  fmt::print("Tx done\n\n");

  std::this_thread::sleep_for(std::chrono::milliseconds(750));

  uint64_t tx_duration_us = duration.count();
  uint64_t tx_bytes       = params.pdu_len * params.nof_pdus;
  uint64_t tx_bits        = tx_bytes * 8;

  fmt::print("Tx time: {} us\n", tx_duration_us);
  fmt::print("Tx PDUs total: {:>7}\n", params.nof_pdus);
  fmt::print("Tx Bytes: {:.3f} GB\n\n", tx_bytes * 1e-9);

  fmt::print("Tx data rate: {:.2f} Mbit/s\n", (long double)(tx_bits) / tx_duration_us);
  fmt::print("Tx PDU rate: {:.2f} PDU/s\n", (long double)params.nof_pdus / (duration.count() * 1e-6));
}
