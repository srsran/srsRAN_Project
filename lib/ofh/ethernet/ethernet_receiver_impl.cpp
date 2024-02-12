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

#include "ethernet_receiver_impl.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/executors/task_executor.h"
#include <arpa/inet.h>
#include <cstring>
#include <future>
#include <net/if.h>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

using namespace srsran;
using namespace ether;

namespace {

class dummy_frame_notifier : public frame_notifier
{
  // See interface for documentation.
  void on_new_frame(span<const uint8_t> payload) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the receiver implementation as a placeholder for the
/// actual frame notifier, which will be later set up through the \ref start() method.
static dummy_frame_notifier dummy_notifier;

receiver_impl::receiver_impl(const std::string&    interface,
                             bool                  is_promiscuous_mode_enabled,
                             task_executor&        executor_,
                             srslog::basic_logger& logger_) :
  logger(logger_), executor(executor_), notifier(dummy_notifier)
{
  socket_fd = ::socket(AF_PACKET, SOCK_RAW, htons(ECPRI_ETH_TYPE));
  if (socket_fd < 0) {
    report_error("Unable to open raw socket for Ethernet receiver: {}", strerror(errno));
  }

  if (interface.size() > (IFNAMSIZ - 1)) {
    report_error("The Ethernet receiver interface name '{}' exceeds the maximum allowed length");
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
  if (::setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, interface.c_str(), interface.size()) == -1) {
    report_error("Unable to bind socket in Ethernet receiver");
  }

  logger.info("Opened successfully the NIC interface '{}' used by the Ethernet receiver", interface);
}

receiver_impl::~receiver_impl()
{
  ::close(socket_fd);
}

void receiver_impl::start(frame_notifier& notifier_)
{
  logger.info("Starting the ethernet frame receiver");

  notifier = std::ref(notifier_);

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (!executor.defer([this, &p]() {
        rx_status.store(receiver_status::running, std::memory_order_relaxed);
        // Signal start() caller thread that the operation is complete.
        p.set_value();
        receive_loop();
      })) {
    report_error("Unable to start the ethernet frame receiver");
  }

  // Block waiting for timing executor to start.
  fut.wait();

  logger.info("Started the ethernet frame receiver");
}

void receiver_impl::stop()
{
  logger.info("Requesting stop of the ethernet frame receiver");
  rx_status.store(receiver_status::stop_requested, std::memory_order_relaxed);

  // Wait for the receiver thread to stop.
  while (rx_status.load(std::memory_order_acquire) != receiver_status::stopped) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  logger.info("Stopped the ethernet frame receiver");
}

void receiver_impl::receive_loop()
{
  if (rx_status.load(std::memory_order_relaxed) == receiver_status::stop_requested) {
    rx_status.store(receiver_status::stopped, std::memory_order_release);
    return;
  }

  receive();

  // Retry the task deferring when it fails.
  while (!executor.defer([this]() { receive_loop(); })) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

/// Blocking function that waits for incoming data over the socket or until the specified timeout expires.
static bool wait_for_data(int socket, std::chrono::seconds timeout)
{
  fd_set read_fs;
  FD_ZERO(&read_fs);
  FD_SET(socket, &read_fs);
  timeval tv = {static_cast<time_t>(timeout.count()), 0};

  return (::select(socket + 1, &read_fs, nullptr, nullptr, &tv) > 0);
}

void receiver_impl::receive()
{
  if (!wait_for_data(socket_fd, std::chrono::seconds(1))) {
    return;
  }

  static constexpr unsigned BUFFER_SIZE = 9600;

  std::array<uint8_t, BUFFER_SIZE> buffer;
  auto                             nof_bytes = ::recvfrom(socket_fd, buffer.data(), BUFFER_SIZE, 0, nullptr, nullptr);

  if (nof_bytes < 0) {
    logger.warning("Ethernet receiver call to recvfrom failed");
    return;
  }

  notifier.get().on_new_frame(span<const uint8_t>(buffer.data(), nof_bytes));
}
