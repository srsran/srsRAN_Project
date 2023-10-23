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

#include "ethernet_receiver_impl.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/executors/task_executor.h"
#include <arpa/inet.h>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace srsran;
using namespace ether;

receiver_impl::receiver_impl(const std::string&    interface,
                             bool                  is_promiscuous_mode_enabled,
                             task_executor&        executor_,
                             frame_notifier&       notifier_,
                             srslog::basic_logger& logger_) :
  logger(logger_), executor(executor_), notifier(notifier_)
{
  socket_fd = ::socket(AF_PACKET, SOCK_RAW, htons(ECPRI_ETH_TYPE));
  if (socket_fd < 0) {
    report_error("Unable to open socket for Ethernet receiver");
  }

  if (is_promiscuous_mode_enabled) {
    // Set interface to promiscuous mode.
    ::ifreq if_opts;
    ::strncpy(if_opts.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    if (::ioctl(socket_fd, SIOCGIFFLAGS, &if_opts) < 0) {
      report_error("Unable to get flags for NIC interface in the Ethernet receiver");
    }
    if_opts.ifr_flags |= IFF_PROMISC;
    if (::ioctl(socket_fd, SIOCSIFFLAGS, &if_opts) < 0) {
      report_error("Unable to set flags for NIC interface in the Ethernet receiver");
    }
  }

  // Bind to device.
  if (::setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, interface.c_str(), IFNAMSIZ - 1) == -1) {
    report_error("Unable to bind socket in Ethernet receiver");
  }

  logger.info("Opened successfully the NIC interface '{}' used by the Ethernet receiver", interface);
}

receiver_impl::~receiver_impl()
{
  ::close(socket_fd);
}

void receiver_impl::start()
{
  logger.info("Starting the ethernet frame receiver");
  executor.defer([this]() { receive_loop(); });
}

void receiver_impl::stop()
{
  logger.info("Requesting stop of the ethernet frame receiver");
  is_stop_requested.store(true, std::memory_order::memory_order_relaxed);
}

void receiver_impl::receive_loop()
{
  receive();

  if (is_stop_requested.load(std::memory_order_relaxed)) {
    return;
  }

  executor.defer([this]() { receive_loop(); });
}

void receiver_impl::receive()
{
  static constexpr unsigned BUFFER_SIZE = 9600;

  std::array<uint8_t, BUFFER_SIZE> buffer;
  auto                             nof_bytes = ::recvfrom(socket_fd, buffer.data(), BUFFER_SIZE, 0, nullptr, nullptr);

  if (nof_bytes < 0) {
    logger.warning("recvfrom failed");
    return;
  }

  notifier.on_new_frame(span<const uint8_t>(buffer.data(), nof_bytes));
}
