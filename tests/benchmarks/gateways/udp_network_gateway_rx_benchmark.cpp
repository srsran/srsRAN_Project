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

/// To run this benchmark, you will need a separate traffic source.
/// As an example, Iperf2 can be used, like so:
/// iperf -c 127.0.0.1 -u -b 5G -p 56701  -P 2 -t 3600 --no-udp-fin

#include "udp_network_gateway_benchmark_helpers.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <queue>

using namespace srsran;

struct bench_params {
  uint64_t    nof_pdus         = 100000;
  unsigned    slow_inter_rx_us = 500;
  unsigned    rx_mmsg          = 256;
  std::string bind_addr        = "127.0.0.1";
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-n <nof PDUs>] [-l <PDU len>] [-u <t us>]\n", prog);
  fmt::print("\t-b Bind address for socket [Default {}]\n", params.bind_addr);
  fmt::print("\t-n Number of PDUs [Default {}]\n", params.nof_pdus);
  fmt::print("\t-u Notify large PDU inter arrival time longer than t microseconds [Default {}]\n",
             params.slow_inter_rx_us);
  fmt::print("\t-m Number of RX PDUs in a syscall (rx_mmsg) [Default {}]\n", params.rx_mmsg);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "b:n:m:u:h")) != -1) {
    switch (opt) {
      case 'b':
        params.bind_addr = std::string(optarg);
        break;
      case 'n':
        params.nof_pdus = std::strtol(optarg, nullptr, 10);
        break;
      case 'm':
        params.rx_mmsg = std::strtol(optarg, nullptr, 10);
        break;
      case 'u':
        params.slow_inter_rx_us = std::strtol(optarg, nullptr, 10);
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

  fmt::print("To run this benchmark, you will need a separate traffic source.\n"
             "As an example, Iperf2 can be used, like so:\n"
             "iperf -c 127.0.0.1 -u -b 5G -p 56701  -P 2 -t 3600 --no-udp-fin\n");

  // init GW logger
  srslog::fetch_basic_logger("IO-EPOLL", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UDP-GW", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UDP-GW", true).set_hex_dump_max_size(100);

  bench_params params{};
  parse_args(argc, argv, params);

  udp_network_gateway_config gw_cfg;
  gw_cfg.bind_address      = params.bind_addr;
  gw_cfg.bind_port         = 56701;
  gw_cfg.non_blocking_mode = false;
  gw_cfg.rx_max_mmsg       = params.rx_mmsg;

  dummy_network_gateway_data_notifier_with_src_addr gw_dn{params.slow_inter_rx_us, params.nof_pdus};
  std::unique_ptr<udp_network_gateway>              gw;

  manual_task_worker io_tx_executor{128};

  gw = create_udp_network_gateway({gw_cfg, gw_dn, io_tx_executor});
  gw->create_and_bind();

  std::unique_ptr<io_broker> epoll_broker;

  epoll_broker = create_io_broker(io_broker_type::epoll);
  bool success = epoll_broker->register_fd(gw->get_socket_fd(), [&gw](int fd) { gw->receive(); });
  if (!success) {
    report_fatal_error("Failed to register UDP network gateway at IO broker. socket_fd={}", gw->get_socket_fd());
  }

  // Wait for all packets to be received
  auto t_start = std::chrono::high_resolution_clock::now();
  while (gw_dn.get_n_pdus() < params.nof_pdus) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  auto t_end    = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
  fmt::print("Rx done\n\n");

  std::this_thread::sleep_for(std::chrono::milliseconds(750));

  uint64_t rx_duration_us = gw_dn.get_t_rx().count();
  uint64_t rx_bytes       = gw_dn.get_rx_bytes();
  uint64_t rx_bits        = rx_bytes * 8;

  fmt::print("Test duration: {} us\n\n", duration.count());
  fmt::print("Rx time: {} us\n\n", rx_duration_us);

  fmt::print("Rx Bytes: {} GB\n", rx_bytes * 1e-9);
  fmt::print("Rx bits: {} b\n\n", rx_bits);

  fmt::print("Rx data rate: {:.2f} Mbit/s\n", (long double)(rx_bits / rx_duration_us));
  fmt::print("Rx PDU rate: {:.2f} PDU/s\n\n", (long double)gw_dn.get_n_pdus() / (rx_duration_us * 1e-6));

  fmt::print("PDU inter arrival time (min/avg/max) [us]: {}/{}/{}\n",
             gw_dn.get_t_min().count(),
             gw_dn.get_t_sum().count() / gw_dn.get_n_pdus(),
             gw_dn.get_t_max().count());
}
