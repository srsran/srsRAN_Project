/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

class dummy_network_gateway_data_notifier_with_src_addr : public network_gateway_data_notifier_with_src_addr
{
public:
  dummy_network_gateway_data_notifier_with_src_addr() = default;
  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    rx_bytes += pdu.length();
    n_pdus++;
  }

  unsigned get_rx_bytes() { return rx_bytes; }
  unsigned get_n_pdus() { return n_pdus; }

private:
  unsigned rx_bytes = 0;
  unsigned n_pdus   = 0;
};

byte_buffer make_tx_byte_buffer(uint32_t length)
{
  byte_buffer pdu{};
  for (uint32_t i = 0; i < length; ++i) {
    pdu.append((uint8_t)i);
  }
  return pdu;
}

byte_buffer make_default_byte_buffer()
{
  return make_tx_byte_buffer(1400);
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
  gw1_cfg.non_blocking_mode = true;

  udp_network_gateway_config gw2_cfg;
  gw2_cfg.bind_address      = "127.0.0.1";
  gw2_cfg.bind_port         = 56702;
  gw2_cfg.non_blocking_mode = true;

  dummy_network_gateway_data_notifier_with_src_addr gw1_dn, gw2_dn;
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

  byte_buffer pdu = make_default_byte_buffer();

  auto t_start = std::chrono::high_resolution_clock::now();

  unsigned N = 100000;
  for (unsigned n = 0; n < N; n++) {
    gw1->handle_pdu(pdu, gw2_addr);
  }
  auto t_end    = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  fmt::print("Tx time: {} us\n", duration.count());
  fmt::print("Tx data rate: {:.2f} Mbit/s\n\n", (double)pdu.length() * N * 8 * 1e-6 / (duration.count() * 1e-6));
  fmt::print("Tx PDU rate: {:.2f} PDU/s\n", (double)N / (duration.count() * 1e-6));
  fmt::print("Rx PDU rate: {:.2f} PDU/s\n\n", (double)gw2_dn.get_n_pdus() / (duration.count() * 1e-6));
  fmt::print("Tx PDUs total: {:>7}\n", N);
  fmt::print("Rx PDUs total: {:>7}\n", gw2_dn.get_n_pdus());
}
