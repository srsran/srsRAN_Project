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

#pragma once

#include "srsran/srslog/srslog.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/sctp_socket.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/sctp.h>

namespace srsran {

/// Dummy IO broker where the registered callbacks have to be called manually.
class dummy_io_broker : public io_broker
{
public:
  bool             accept_next_fd = true;
  unique_fd        last_registered_fd;
  recv_callback_t  handle_receive;
  error_callback_t handle_error;
  int              last_unregistered_fd = -1;

  [[nodiscard]] subscriber register_fd(
      unique_fd        fd,
      task_executor&   executor,
      recv_callback_t  handler_,
      error_callback_t err_handler_ = [](error_code) {}) override
  {
    last_registered_fd = std::move(fd);
    if (not accept_next_fd) {
      return {};
    }
    handle_receive = handler_;
    handle_error   = err_handler_;
    return subscriber{*this, last_registered_fd.value()};
  }

  [[nodiscard]] bool unregister_fd(int fd, std::promise<bool>* complete_notifier) override
  {
    last_unregistered_fd = fd;
    handle_receive       = {};
    handle_error         = {};
    if (complete_notifier) {
      complete_notifier->set_value(true);
    }
    return true;
  }
};

struct test_recv_data {
  struct sctp_sndrcvinfo sri       = {};
  int                    msg_flags = 0;
  sockaddr_storage       msg_src_addr;
  // fromlen is an in/out variable in sctp_recvmsg.
  socklen_t            msg_src_addrlen = sizeof(msg_src_addr);
  std::vector<uint8_t> data;

  bool has_notification() const { return msg_flags & MSG_NOTIFICATION; }
  bool has_data() const { return not has_notification(); }
  int  sctp_notification() const
  {
    srsran_assert(has_notification(), "bad access");
    const auto* notif = reinterpret_cast<const union sctp_notification*>(data.data());
    return notif->sn_header.sn_type;
  }
  const struct sctp_assoc_change& sctp_assoc_change() const
  {
    srsran_assert(has_notification() and sctp_notification() == SCTP_ASSOC_CHANGE, "bad access");
    const auto* notif = reinterpret_cast<const union sctp_notification*>(data.data());
    return notif->sn_assoc_change;
  }
};

class dummy_sctp_node
{
public:
  dummy_sctp_node(const std::string& name_) : name(name_), logger(srslog::fetch_basic_logger(name))
  {
    logger.set_level(srslog::basic_levels::debug);
  }
  ~dummy_sctp_node() { close(); }

  bool close()
  {
    if (socket.is_open()) {
      socket.close();
      logger.info("{} shut down", name);
      return true;
    }
    return false;
  }

  std::optional<test_recv_data> receive()
  {
    static constexpr uint32_t network_gateway_sctp_max_len = 9100;

    test_recv_data data;

    std::array<uint8_t, network_gateway_sctp_max_len> temp_buf;
    int                                               rx_bytes = ::sctp_recvmsg(socket.fd().value(),
                                  temp_buf.data(),
                                  temp_buf.size(),
                                  (struct sockaddr*)&data.msg_src_addr,
                                  &data.msg_src_addrlen,
                                  &data.sri,
                                  &data.msg_flags);
    if (rx_bytes < 0) {
      if (errno != EAGAIN) {
        logger.error("Recv error: {}", ::strerror(errno));
      }
      return std::nullopt;
    }

    data.data.assign(temp_buf.begin(), temp_buf.begin() + rx_bytes);
    return data;
  }

  bool send_data(const std::vector<uint8_t>& bytes, int ppid, const sockaddr& dest_addr, socklen_t dest_addrlen)
  {
    int bytes_sent = ::sctp_sendmsg(socket.fd().value(),
                                    bytes.data(),
                                    bytes.size(),
                                    (struct sockaddr*)&dest_addr,
                                    dest_addrlen,
                                    htonl(ppid),
                                    0,
                                    0,
                                    0,
                                    0);
    return bytes_sent == (int)bytes.size();
  }

  bool send_data(const std::vector<uint8_t>& bytes, int ppid, std::string dest_addr, int dest_port)
  {
    sockaddr_in addr     = {};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(dest_port);
    addr.sin_addr.s_addr = ::inet_addr(dest_addr.c_str());
    return send_data(bytes, ppid, (struct sockaddr&)addr, sizeof(addr));
  }

  bool send_eof(int ppid, const sockaddr& dest_addr, socklen_t dest_addrlen)
  {
    // Send EOF to SCTP server.
    int bytes_sent = ::sctp_sendmsg(socket.fd().value(),
                                    nullptr,
                                    0,
                                    const_cast<struct sockaddr*>(&dest_addr),
                                    dest_addrlen,
                                    htonl(ppid),
                                    SCTP_EOF,
                                    0,
                                    0,
                                    0);
    return bytes_sent != -1;
  }

  sctp_socket           socket;
  std::string           name;
  srslog::basic_logger& logger;
};

} // namespace srsran
