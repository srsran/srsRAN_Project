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

#include "test_helpers.h"
#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/support/executors/manual_task_worker.h"

using namespace srsran;

constexpr uint32_t pool_size = 128;

class udp_pool_network_gateway_tester : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // init GW logger
    srslog::fetch_basic_logger("UDP-GW", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("UDP-GW", false).set_hex_dump_max_size(100);

    tx_sock_fd = unique_fd{::socket(AF_INET, SOCK_DGRAM, 0)};
    ASSERT_TRUE(tx_sock_fd.is_open());

    sockaddr_in tx_addr = {};
    tx_addr.sin_family  = AF_INET;
    tx_addr.sin_port    = ::htons(0);

    ASSERT_EQ(::inet_pton(AF_INET, client_address_v4.data(), &tx_addr.sin_addr), 1);
    ASSERT_EQ(::bind(tx_sock_fd.value(), (sockaddr*)&tx_addr, sizeof(tx_addr)), 0);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();

    stop_token.store(true, std::memory_order_relaxed);
    if (rx_thread.joinable()) {
      rx_thread.join();
    }
  }

  void set_config(udp_network_gateway_config server_config)
  {
    server =
        create_udp_network_gateway({std::move(server_config), server_data_notifier, io_tx_executor, io_tx_executor});
    ASSERT_NE(server, nullptr);
  }

  // spawn a thread to receive data
  void start_receive_thread()
  {
    rx_thread = std::thread([this]() {
      stop_token.store(false);
      while (not stop_token.load(std::memory_order_relaxed)) {
        // call receive() on socket
        server->receive();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
    });
  }

  void send_to_server(const std::string& dest_addr, uint16_t port)
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
    } else {
      FAIL();
    }

    std::array<uint8_t, 16> buffer = {};
    ::sendto(tx_sock_fd.value(),
             buffer.data(),
             buffer.size(),
             0,
             reinterpret_cast<const struct sockaddr*>(&addr_storage),
             sizeof(addr_storage));
  }

  dummy_network_gateway_control_notifier server_control_notifier;
  dummy_network_gateway_control_notifier client_control_notifier;

  dummy_network_gateway_data_notifier_with_src_addr server_data_notifier;
  dummy_network_gateway_data_notifier_with_src_addr client_data_notifier;

  std::unique_ptr<udp_network_gateway> server;

  manual_task_worker io_tx_executor{128};

  std::string server_address_v4 = "127.0.0.1";
  std::string client_address_v4 = "127.0.1.1";

private:
  std::thread       rx_thread;
  std::atomic<bool> stop_token = {false};
  unique_fd tx_sock_fd; // we use a normal socket for transmission of packets instead of another UDP GW, to avoid having
                        // to use byte buffers for TX and having multiple threads allocating from the buffer pool
};

TEST_F(udp_pool_network_gateway_tester, when_config_valid_then_trx_succeeds)
{
  // create server config
  udp_network_gateway_config server_config;
  server_config.bind_address             = server_address_v4;
  server_config.bind_port                = 0;
  server_config.non_blocking_mode        = false;
  server_config.pool_occupancy_threshold = 0.5;

  // set configs
  set_config(server_config);

  // create and bind gateways
  ASSERT_TRUE(server->create_and_bind());

  std::string server_address = {};
  ASSERT_TRUE(server->get_bind_address(server_address));
  std::optional<uint16_t> server_port = server->get_bind_port();
  ASSERT_TRUE(server_port.has_value());
  start_receive_thread();

  unsigned                 n_pdus = 64;
  std::vector<byte_buffer> pdu_list;
  for (unsigned i = 0; i < n_pdus; i++) {
    send_to_server(server_address_v4, server_port.value());
    expected<byte_buffer> rx_pdu = server_data_notifier.get_rx_pdu_blocking();
    ASSERT_TRUE(rx_pdu.has_value());

    pdu_list.push_back(std::move(rx_pdu.value())); // move pdus to keep pool full
  }

  send_to_server(server_address_v4, server_port.value());
  // No PDU expected
  expected<byte_buffer> rx_pdu = server_data_notifier.get_rx_pdu_blocking(std::chrono::milliseconds(200));
  ASSERT_FALSE(rx_pdu.has_value()); // should not have been received
}

int main(int argc, char** argv)
{
  init_byte_buffer_segment_pool(pool_size);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
