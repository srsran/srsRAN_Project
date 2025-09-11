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

#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/io/unique_fd.h"
#include <condition_variable>
#include <future>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h> // for unix sockets

using namespace srsran;

static const std::string tx_buf = "hello world!";

class io_broker_epoll : public ::testing::Test
{
protected:
  io_broker_epoll()
  {
    srslog::init();
    epoll_broker = create_io_broker(io_broker_type::epoll);
  }

  ~io_broker_epoll() override { srslog::flush(); }

  void data_receive_callback()
  {
    // Receive data on provided fd.
    char rx_buf[1024];
    int  bytes = ::read(raw_socket_fd, rx_buf, sizeof(rx_buf));

    std::lock_guard<std::mutex> lock(rx_mutex);
    total_rx_bytes += bytes;

    if (socket_type == SOCK_DGRAM) {
      ASSERT_EQ(bytes, tx_buf.length());
    }
    rx_cvar.notify_one();
  }

  void error_callback(io_broker::error_code code) { ++error_count; }

  void create_unix_sockets()
  {
    // Check temporary files for IPC.
    std::string socket_filename = "/tmp/io_broker_server";
    ASSERT_LE(socket_filename.size(), sizeof(sockaddr_un::sun_path));

    // Delete existing file (if any).
    int ret = ::remove(socket_filename.c_str());
    if (ret == -1 && errno != ENOENT) {
      // Allow ENOENT, i.e. file not found.
      ASSERT_NE(ret, -1);
    }

    // Create server socket.
    socket_fd   = unique_fd{::socket(AF_UNIX, SOCK_DGRAM, 0)};
    socket_type = SOCK_DGRAM;
    ASSERT_TRUE(socket_fd.is_open());
    raw_socket_fd = socket_fd.value();

    // Prepare server address.
    server_addr_un.sun_family = AF_UNIX;
    std::strncpy(server_addr_un.sun_path, socket_filename.c_str(), socket_filename.length());

    // Bind server.
    ret = ::bind(raw_socket_fd, reinterpret_cast<::sockaddr*>(&server_addr_un), sizeof(server_addr_un));
    ASSERT_NE(ret, -1);

    // Prepare client address.
    client_addr_un.sun_family = AF_UNIX;
    std::strncpy(client_addr_un.sun_path, socket_filename.c_str(), socket_filename.length());

    // Connect client to server_filename.
    ret = ::connect(raw_socket_fd, reinterpret_cast<::sockaddr*>(&client_addr_un), sizeof(client_addr_un));
    ASSERT_NE(ret, -1);
  }

  std::optional<uint16_t> get_bind_port(int sock_fd)
  {
    if (sock_fd == -1) {
      return std::nullopt;
    }

    ::sockaddr_storage gw_addr_storage;
    auto*              gw_addr     = reinterpret_cast<::sockaddr*>(&gw_addr_storage);
    ::socklen_t        gw_addr_len = sizeof(gw_addr_storage);

    int ret = ::getsockname(sock_fd, gw_addr, &gw_addr_len);
    if (ret != 0) {
      return std::nullopt;
    }

    uint16_t bind_port;
    if (gw_addr->sa_family == AF_INET) {
      bind_port = ntohs(reinterpret_cast<::sockaddr_in*>(gw_addr)->sin_port);
    } else if (gw_addr->sa_family == AF_INET6) {
      bind_port = ntohs(reinterpret_cast<::sockaddr_in6*>(gw_addr)->sin6_port);
    } else {
      return std::nullopt;
    }

    return bind_port;
  }

  void create_af_init_sockets(int type)
  {
    // Create server socket.
    socket_fd   = unique_fd{::socket(AF_INET, type, 0)};
    socket_type = type;
    ASSERT_TRUE(socket_fd.is_open());
    raw_socket_fd = socket_fd.value();

    // Configure socket as reusable to allow multiple runs.
    int enable = 1;
    ASSERT_NE(::setsockopt(raw_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)), -1);

    // prepare server address
    std::memset(&server_addr_in, 0, sizeof(::sockaddr_in));
    server_addr_in.sin_family      = AF_INET;
    server_addr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr_in.sin_port        = htons(0);

    // Bind server.
    int ret = ::bind(raw_socket_fd, reinterpret_cast<::sockaddr*>(&server_addr_in), sizeof(server_addr_in));
    ASSERT_NE(ret, -1);

    // Get bind port.
    std::optional<uint16_t> port = get_bind_port(raw_socket_fd);
    ASSERT_TRUE(port.has_value());
    ASSERT_NE(port.value(), 0);
    // Update server address.
    server_addr_in.sin_port = htons(port.value());

    // Prepare client address.
    std::memset(&client_addr_in, 0, sizeof(::sockaddr_in));
    client_addr_in.sin_family      = AF_INET;
    client_addr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    client_addr_in.sin_port        = htons(port.value());

    // Connect client to server
    ret = ::connect(raw_socket_fd, reinterpret_cast<::sockaddr*>(&server_addr_in), sizeof(server_addr_in));
    ASSERT_NE(ret, -1);
  }

  void add_socket_to_epoll()
  {
    fd_handle = epoll_broker->register_fd(
        std::move(socket_fd),
        executor,
        [this]() { data_receive_callback(); },
        [this](io_broker::error_code code) { error_callback(code); });
    ASSERT_TRUE(fd_handle.registered());
  }

  void rem_socket_from_epoll() { EXPECT_TRUE(fd_handle.reset()); }

  void send_on_socket() const
  {
    // Send text.
    int ret = ::send(raw_socket_fd, tx_buf.c_str(), tx_buf.length(), 0);
    ASSERT_EQ(ret, tx_buf.length());
  }

  void run_tx_rx_test()
  {
    const unsigned count = 5;
    int            run   = count;
    while (run-- > 0) {
      send_on_socket();
    }

    // wait until all bytes are received
    std::unique_lock<std::mutex> lock(rx_mutex);
    rx_cvar.wait(lock, [this]() { return total_rx_bytes >= tx_buf.length() * count; });
    ASSERT_EQ(total_rx_bytes, tx_buf.length() * count);
  }

  inline_task_executor       executor;
  std::unique_ptr<io_broker> epoll_broker;
  int                        raw_socket_fd = -1;
  unique_fd                  socket_fd;
  int                        socket_type = 0;

  io_broker::subscriber fd_handle;

  // Unix domain socket addresses (used by unix sockets only).
  ::sockaddr_un server_addr_un = {};
  ::sockaddr_un client_addr_un = {};

  // Inet socket addresses (used by all other protocols)
  ::sockaddr_in server_addr_in = {};
  ::sockaddr_in client_addr_in = {};

  std::mutex              rx_mutex;
  std::condition_variable rx_cvar;

  std::atomic<int> error_count{0};

  size_t total_rx_bytes = 0;
};

TEST_F(io_broker_epoll, unix_socket_trx_test)
{
  create_unix_sockets();
  add_socket_to_epoll();
  run_tx_rx_test();
  rem_socket_from_epoll();
}

TEST_F(io_broker_epoll, af_inet_socket_udp_trx_test)
{
  create_af_init_sockets(SOCK_DGRAM);
  add_socket_to_epoll();
  run_tx_rx_test();
  rem_socket_from_epoll();
}

TEST_F(io_broker_epoll, af_inet_socket_tcp_trx_test)
{
  create_af_init_sockets(SOCK_STREAM);
  add_socket_to_epoll();
  run_tx_rx_test();
  rem_socket_from_epoll();
}

TEST_F(io_broker_epoll, reentrant_handle_and_deregistration)
{
  create_unix_sockets();

  std::promise<bool>    p;
  std::future<bool>     fut = p.get_future();
  io_broker::subscriber handle;

  handle = this->epoll_broker->register_fd(std::move(socket_fd), executor, [&]() {
    auto* p_copy = &p;
    bool  ret    = handle.reset();
    p_copy->set_value(ret);
  });
  ASSERT_TRUE(handle.registered());

  send_on_socket();

  ASSERT_EQ(fut.wait_for(std::chrono::seconds{100}), std::future_status::ready);
  ASSERT_TRUE(fut.get());
}

TEST_F(io_broker_epoll, error_callback_called_when_epollhup)
{
  // Create pipe
  int pipefd[2];
  ASSERT_EQ(::pipe(pipefd), 0);

  std::promise<void> p;
  std::future<void>  f = p.get_future();

  // Subscribe pipe fd.
  auto sub = this->epoll_broker->register_fd(
      unique_fd(pipefd[0]),
      executor,
      []() {},
      [this, &p](io_broker::error_code code) {
        ++this->error_count;
        p.set_value();
      });
  ASSERT_TRUE(sub.registered());

  // Close pipe, while subscribed. This will cause an EPOLLHUP event.
  ASSERT_EQ(this->error_count, 0);
  ::close(pipefd[1]);

  // Check if the error handler was called. The error can take some time to trigger the epoll.
  f.wait();
  ASSERT_EQ(this->error_count, 1);
}
