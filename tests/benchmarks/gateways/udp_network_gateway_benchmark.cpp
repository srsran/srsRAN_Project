/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/io/io_broker_factory.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <queue>

using namespace srsran;

struct bench_params {
  unsigned pdu_len          = 1400;
  unsigned nof_pdus         = 100000;
  unsigned slow_inter_rx_us = 500;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-n <nof PDUs>] [-l <PDU len>] [-u <t us>]\n", prog);
  fmt::print("\t-l PDU len [Default {}]\n", params.pdu_len);
  fmt::print("\t-n Number of PDUs [Default {}]\n", params.nof_pdus);
  fmt::print("\t-u Notify large PDU inter arrival time longer than t microseconds [Default {}]\n",
             params.slow_inter_rx_us);
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

  unsigned get_rx_bytes() { return rx_bytes; }
  unsigned get_n_pdus() { return n_pdus; }

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
  ;
};

byte_buffer make_tx_byte_buffer(uint32_t length)
{
  byte_buffer pdu{};
  for (uint32_t i = 0; i < length; ++i) {
    pdu.append((uint8_t)i);
  }
  return pdu;
}

sockaddr_storage to_sockaddr_storage(std::string dest_addr, uint16_t port)
{
  in_addr          inaddr_v4    = {};
  in6_addr         inaddr_v6    = {};
  sockaddr_storage addr_storage = {};

  if (inet_pton(AF_INET, dest_addr.c_str(), &inaddr_v4) == 1) {
    sockaddr_in* tmp = (sockaddr_in*)&addr_storage;
    tmp->sin_family  = AF_INET;
    tmp->sin_addr    = inaddr_v4;
    tmp->sin_port    = htons(port);
  } else if (inet_pton(AF_INET6, dest_addr.c_str(), &inaddr_v6) == 1) {
    sockaddr_in6* tmp = (sockaddr_in6*)&addr_storage;
    tmp->sin6_family  = AF_INET6;
    tmp->sin6_addr    = inaddr_v6;
    tmp->sin6_port    = htons(port);
  }
  return addr_storage;
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

  udp_network_gateway_config gw2_cfg;
  gw2_cfg.bind_address      = "127.0.0.1";
  gw2_cfg.bind_port         = 56702;
  gw2_cfg.non_blocking_mode = false;

  dummy_network_gateway_data_notifier_with_src_addr gw1_dn{params}, gw2_dn{params};
  std::unique_ptr<udp_network_gateway>              gw1, gw2;

  gw1 = create_udp_network_gateway({gw1_cfg, gw1_dn});
  gw2 = create_udp_network_gateway({gw2_cfg, gw2_dn});

  gw1->create_and_bind();
  gw2->create_and_bind();

  std::unique_ptr<io_broker> epoll_broker;

  epoll_broker = create_io_broker(io_broker_type::epoll);

  epoll_broker->register_fd(gw1->get_socket_fd(), [&gw1](int fd) { gw1->receive(); });
  epoll_broker->register_fd(gw2->get_socket_fd(), [&gw2](int fd) { gw2->receive(); });

  sockaddr_storage gw2_addr = to_sockaddr_storage(gw2_cfg.bind_address, gw2_cfg.bind_port);

  byte_buffer pdu = make_tx_byte_buffer(params.pdu_len);

  auto t_start = std::chrono::high_resolution_clock::now();

  unsigned nof_pdus = params.nof_pdus;
  for (unsigned n = 0; n < nof_pdus; n++) {
    gw1->handle_pdu(pdu, gw2_addr);
  }
  auto t_end    = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
  fmt::print("Tx done\n\n");

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  fmt::print("Tx time: {} us\n", duration.count());
  fmt::print("Tx data rate: {:.2f} Mbit/s\n", (double)pdu.length() * nof_pdus * 8 * 1e-6 / (duration.count() * 1e-6));
  fmt::print("Rx data rate: {:.2f} Mbit/s\n\n",
             (double)pdu.length() * gw2_dn.get_n_pdus() * 8 * 1e-6 / (duration.count() * 1e-6));

  fmt::print("Tx PDU rate: {:.2f} PDU/s\n", (double)nof_pdus / (duration.count() * 1e-6));
  fmt::print("Rx PDU rate: {:.2f} PDU/s\n\n", (double)gw2_dn.get_n_pdus() / (duration.count() * 1e-6));

  fmt::print("Tx PDUs total: {:>7}\n", nof_pdus);
  fmt::print("Rx PDUs total: {:>7} ({:.2f}% lost)\n\n",
             gw2_dn.get_n_pdus(),
             (1 - ((double)gw2_dn.get_n_pdus() / nof_pdus)) * 100);

  fmt::print("PDU inter arrival time (min/avg/max) [us]: {}/{}/{}\n",
             gw2_dn.get_t_min().count(),
             gw2_dn.get_t_sum().count() / gw2_dn.get_n_pdus(),
             gw2_dn.get_t_max().count());
}
