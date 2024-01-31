/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/io/io_broker_factory.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <queue>

using namespace srsran;

struct bench_params {
  unsigned nof_pdus         = 100000;
  unsigned slow_inter_rx_us = 500;
  unsigned rx_mmsg          = 256;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-n <nof PDUs>] [-l <PDU len>] [-u <t us>]\n", prog);
  fmt::print("\t-n Number of PDUs [Default {}]\n", params.nof_pdus);
  fmt::print("\t-u Notify large PDU inter arrival time longer than t microseconds [Default {}]\n",
             params.slow_inter_rx_us);
  fmt::print("\t-m Number of RX PDUs in a syscall (rx_mmsg) [Default {}]\n", params.rx_mmsg);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "l:n:m:u:h")) != -1) {
    switch (opt) {
      case 'n':
        params.nof_pdus = std::strtol(optarg, nullptr, 10);
        break;
      case 'm':
        params.rx_mmsg = std::strtol(optarg, nullptr, 10);
        break;
      case 'u':
        params.slow_inter_rx_us = std::strtol(optarg, nullptr, 10);
      case 'h':
      default:
        usage(argv[0], params);
        exit(0);
    }
  }
}

class dummy_network_gateway_data_notifier_with_src_addr : public network_gateway_data_notifier_with_src_addr
{
public:
  dummy_network_gateway_data_notifier_with_src_addr(const bench_params& params_) : params(params_) {}

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    rx_bytes += pdu.length();
    n_pdus++;

    static bool first = true;
    auto        t_now = std::chrono::high_resolution_clock::now();
    if (!first) {
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_now - t_last);
      if (duration < t_min) {
        t_min = duration;
      }
      if (duration > t_max) {
        t_max = duration;
      }
      t_sum += duration;
      if (duration.count() > params.slow_inter_rx_us) {
        fmt::print("Long inter Rx interval t={}us at n_pdus={}\n", duration.count(), n_pdus);
      }
    } else {
      first = false;
    }

    t_last = t_now;
  }

  unsigned get_rx_bytes() const { return rx_bytes; }
  unsigned get_n_pdus() const { return n_pdus; }

  std::chrono::microseconds get_t_min() { return t_min; }
  std::chrono::microseconds get_t_max() { return t_max; }
  std::chrono::microseconds get_t_sum() { return t_sum; }

private:
  const bench_params& params;

  unsigned rx_bytes = 0;
  unsigned n_pdus   = 0;

  std::chrono::high_resolution_clock::time_point t_last = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds                      t_min  = std::chrono::microseconds::max();
  std::chrono::microseconds                      t_max  = std::chrono::microseconds::min();
  std::chrono::microseconds                      t_sum  = std::chrono::microseconds::zero();
};

int main(int argc, char** argv)
{
  srslog::init();

  // init GW logger
  srslog::fetch_basic_logger("UDP-GW", true).set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("UDP-GW", true).set_hex_dump_max_size(100);

  bench_params params{};
  parse_args(argc, argv, params);

  udp_network_gateway_config gw_cfg;
  gw_cfg.bind_address      = "127.0.0.1";
  gw_cfg.bind_port         = 56701;
  gw_cfg.non_blocking_mode = false;
  gw_cfg.rx_max_mmsg       = 16;

  dummy_network_gateway_data_notifier_with_src_addr gw_dn{params};
  std::unique_ptr<udp_network_gateway>              gw;

  manual_task_worker io_tx_executor{128};

  gw = create_udp_network_gateway({gw_cfg, gw_dn, io_tx_executor});
  gw->create_and_bind();

  std::unique_ptr<io_broker> epoll_broker;

  epoll_broker = create_io_broker(io_broker_type::epoll);
  epoll_broker->register_fd(gw->get_socket_fd(), [&gw](int fd) { gw->receive(); });

  // Wait for all packets to be received
  auto t_start = std::chrono::high_resolution_clock::now();
  while (gw_dn.get_n_pdus() < params.nof_pdus) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  auto t_end    = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
  fmt::print("Rx done\n\n");

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  fmt::print("Test duration: {} us\n", duration.count());
  fmt::print("Rx data rate: {:.2f} Mbit/s\n\n",
             (double)gw_dn.get_rx_bytes() * 8 * 1e-6 / (gw_dn.get_t_sum().count() * 1e-6));

  fmt::print("Rx PDU rate: {:.2f} PDU/s\n\n", (double)gw_dn.get_n_pdus() / (gw_dn.get_t_sum().count() * 1e-6));

  fmt::print("PDU inter arrival time (min/avg/max) [us]: {}/{}/{}\n",
             gw_dn.get_t_min().count(),
             gw_dn.get_t_sum().count() / gw_dn.get_n_pdus(),
             gw_dn.get_t_max().count());
}
