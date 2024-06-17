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

#include "srsran/adt/optional.h"
#include "srsran/srslog/srslog.h"
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
  ~io_broker_epoll() { srslog::flush(); }

  void data_receive_callback()
  {
    // receive data on provided fd
    char rx_buf[1024];
    int  bytes = read(socket_fd.value(), rx_buf, sizeof(rx_buf));

    std::lock_guard<std::mutex> lock(rx_mutex);
    total_rx_bytes += bytes;

    if (socket_type == SOCK_DGRAM) {
      ASSERT_EQ(bytes, tx_buf.length());
    }
    rx_cvar.notify_one();
  }

  void error_callback(io_broker::error_code code) { error_count++; }

  void create_unix_sockets()
  {
    // check temporary files for IPC
    std::string socket_filename = "/tmp/io_broker_server";
    ASSERT_LE(socket_filename.size(), sizeof(sockaddr_un::sun_path));

    // delete exsiting file (if any)
    int ret = remove(socket_filename.c_str());
    if (ret == -1 && errno != ENOENT) {
      // allow ENOENT, i.e. file not found
      ASSERT_NE(ret, -1);
    }

    // create server socket
    socket_fd   = unique_fd{socket(AF_UNIX, SOCK_DGRAM, 0)};
    socket_type = SOCK_DGRAM;
    ASSERT_TRUE(socket_fd.is_open());

    // prepare server address
    // memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr_un.sun_family = AF_UNIX;
    strncpy(server_addr_un.sun_path, socket_filename.c_str(), socket_filename.length());

    // bind server
    ret = bind(socket_fd.value(), (struct sockaddr*)&server_addr_un, sizeof(server_addr_un));
    ASSERT_NE(ret, -1);

    // listen+accept?
    // ret = listen(socket_fd,1);
    // ASSERT_NE(ret, -1);

    // prepare client address
    client_addr_un.sun_family = AF_UNIX;
    strncpy(client_addr_un.sun_path, socket_filename.c_str(), socket_filename.length());

    // connect client to server_filename
    ret = connect(socket_fd.value(), (struct sockaddr*)&client_addr_un, sizeof(client_addr_un));
    // perror("socket failed");
    ASSERT_NE(ret, -1);
  }

  std::optional<uint16_t> get_bind_port(int sock_fd)
  {
    if (sock_fd == -1) {
      return {};
    }

    sockaddr_storage gw_addr_storage;
    sockaddr*        gw_addr     = (sockaddr*)&gw_addr_storage;
    socklen_t        gw_addr_len = sizeof(gw_addr_storage);

    int ret = getsockname(sock_fd, gw_addr, &gw_addr_len);
    if (ret != 0) {
      return {};
    }

    uint16_t bind_port;
    if (gw_addr->sa_family == AF_INET) {
      bind_port = ntohs(((sockaddr_in*)gw_addr)->sin_port);
    } else if (gw_addr->sa_family == AF_INET6) {
      bind_port = ntohs(((sockaddr_in6*)gw_addr)->sin6_port);
    } else {
      return {};
    }

    return bind_port;
  }

  void create_af_init_sockets(int type)
  {
    // create server socket
    socket_fd   = unique_fd{socket(AF_INET, type, 0)};
    socket_type = type;
    ASSERT_TRUE(socket_fd.is_open());

    // configure socket as reusable to allow multiple runs
    int enable = 1;
    ASSERT_NE(setsockopt(socket_fd.value(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)), -1);

    // prepare server address
    memset(&server_addr_in, 0, sizeof(struct sockaddr_in));
    server_addr_in.sin_family      = AF_INET;
    server_addr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr_in.sin_port        = htons(0);

    // bind server
    int ret = bind(socket_fd.value(), (struct sockaddr*)&server_addr_in, sizeof(server_addr_in));
    // perror("socket failed");
    ASSERT_NE(ret, -1);

    // get bind port
    std::optional<uint16_t> port = get_bind_port(socket_fd.value());
    ASSERT_TRUE(port.has_value());
    ASSERT_NE(port.value(), 0);
    // update server address
    server_addr_in.sin_port = htons(port.value());

    // prepare client address
    memset(&client_addr_in, 0, sizeof(struct sockaddr_in));
    client_addr_in.sin_family      = AF_INET;
    client_addr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    client_addr_in.sin_port        = htons(port.value());

    // connect client to server
    ret = connect(socket_fd.value(), (struct sockaddr*)&server_addr_in, sizeof(server_addr_in));
    ASSERT_NE(ret, -1);
  }

  void add_socket_to_epoll()
  {
    fd_handle = epoll_broker->register_fd(
        socket_fd.value(),
        [this]() { data_receive_callback(); },
        [this](io_broker::error_code code) { error_callback(code); });
    ASSERT_TRUE(fd_handle.registered());
  }

  void rem_socket_from_epoll()
  {
    if (socket_fd.is_open()) {
      EXPECT_TRUE(fd_handle.reset());
      ASSERT_TRUE(socket_fd.close());
    }
  }

  void send_on_socket() const
  {
    // send text
    int ret = send(socket_fd.value(), tx_buf.c_str(), tx_buf.length(), 0);
    ASSERT_EQ(ret, tx_buf.length());
  }

  void run_tx_rx_test()
  {
    const int count = 5;
    int       run   = count;
    while (run-- > 0) {
      send_on_socket();
    }

    // wait until all bytes are received
    std::unique_lock<std::mutex> lock(rx_mutex);
    rx_cvar.wait(lock, [this]() { return total_rx_bytes >= tx_buf.length() * count; });
    ASSERT_EQ(total_rx_bytes, tx_buf.length() * count);
  }

  std::unique_ptr<io_broker> epoll_broker;
  unique_fd                  socket_fd;
  int                        socket_type = 0;

  io_broker::subscriber fd_handle;

  // unix domain socket addresses (used by unix sockets only)
  struct sockaddr_un server_addr_un = {};
  struct sockaddr_un client_addr_un = {};

  // inet socket addresses (used by all other protocols)
  struct sockaddr_in server_addr_in = {};
  struct sockaddr_in client_addr_in = {};

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

  handle = this->epoll_broker->register_fd(socket_fd.value(), [&]() {
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
  ASSERT_EQ(pipe(pipefd), 0);

  std::promise<void> p;
  std::future<void>  f = p.get_future();

  // Subscribe pipe fd.
  auto sub = this->epoll_broker->register_fd(
      pipefd[0],
      []() {},
      [this, &p](io_broker::error_code code) {
        this->error_count++;
        p.set_value();
      });
  ASSERT_TRUE(sub.registered());

  // Close pipe, while subscribed. This will cause an EPOLLHUP event.
  ASSERT_EQ(this->error_count, 0);
  close(pipefd[1]);

  // Check if the error handler was called. The error can take some time to trigger the epoll.
  f.wait();
  ASSERT_EQ(this->error_count, 1);
}
