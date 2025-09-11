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

#include "ethernet_receiver_impl.h"
#include "ethernet_rx_buffer_impl.h"
#include "srsran/instrumentation/traces/ofh_traces.h"
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
  void on_new_frame(ether::unique_rx_buffer buffer) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the receiver implementation as a placeholder for the
/// actual frame notifier, which will be later set up through the \ref start() method.
static dummy_frame_notifier dummy_notifier;

receiver_impl::receiver_impl(const receiver_config& config, task_executor& executor_, srslog::basic_logger& logger_) :
  logger(logger_),
  executor(executor_),
  notifier(&dummy_notifier),
  buffer_pool(BUFFER_SIZE),
  metrics_collector(config.are_metrics_enabled)
{
  socket_fd = ::socket(AF_PACKET, SOCK_RAW, htons(ECPRI_ETH_TYPE));
  if (socket_fd < 0) {
    report_error("Unable to open raw socket for Ethernet receiver: {}", ::strerror(errno));
  }

  if (config.interface.size() > (IFNAMSIZ - 1)) {
    report_error("The Ethernet receiver interface name '{}' exceeds the maximum allowed length", config.interface);
  }

  if (config.is_promiscuous_mode_enabled) {
    // Set interface to promiscuous mode.
    ::ifreq if_opts;
    ::strncpy(if_opts.ifr_name, config.interface.c_str(), IFNAMSIZ - 1);
    if (::ioctl(socket_fd, SIOCGIFFLAGS, &if_opts) < 0) {
      report_error("Unable to get flags for NIC interface '{}' in the Ethernet receiver", config.interface);
    }
    if_opts.ifr_flags |= IFF_PROMISC;
    if (::ioctl(socket_fd, SIOCSIFFLAGS, &if_opts) < 0) {
      report_error("Unable to set flags for NIC interface '{}' in the Ethernet receiver", config.interface);
    }
  }

  // Bind to device.
  if (::setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, config.interface.c_str(), config.interface.size()) == -1) {
    report_error("Unable to bind socket to the NIC interface '{}' in Ethernet receiver", config.interface);
  }

  logger.info("Opened successfully the NIC interface '{}' (fd = '{}') used by the Ethernet receiver",
              config.interface,
              socket_fd);
}

receiver_impl::~receiver_impl()
{
  ::close(socket_fd);
}

void receiver_impl::start(frame_notifier& notifier_)
{
  stop_manager.reset();

  logger.info("Starting the ethernet frame receiver");

  notifier = &notifier_;

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (!executor.defer([this, &p]() {
        // Signal to the start() caller thread that the operation is complete.
        p.set_value();
        receive_loop();
      })) {
    report_error("Unable to start the ethernet frame receiver, fd = '{}'", socket_fd);
  }

  // Block waiting for timing executor to start.
  fut.wait();

  logger.info("Started the ethernet frame receiver with fd = '{}'", socket_fd);
}

void receiver_impl::stop()
{
  logger.info("Requesting stop of the ethernet frame receiver with fd = '{}'", socket_fd);

  stop_manager.stop();

  logger.info("Stopped the ethernet frame receiver with fd = '{}'", socket_fd);
}

void receiver_impl::receive_loop()
{
  if (stop_manager.stop_was_requested()) {
    return;
  }

  receive();

  // Retry the task deferring when it fails.
  while (!executor.defer([this, token = stop_manager.get_token()]() { receive_loop(); })) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

/// Blocking function that waits for incoming data over the socket or until the specified timeout expires.
static bool wait_for_data(int socket, std::chrono::microseconds timeout)
{
  fd_set read_fs;
  FD_ZERO(&read_fs);
  FD_SET(socket, &read_fs);
  timeval tv = {0, static_cast<__suseconds_t>(timeout.count())};

  return (::select(socket + 1, &read_fs, nullptr, nullptr, &tv) > 0);
}

void receiver_impl::receive()
{
  if (!wait_for_data(socket_fd, std::chrono::microseconds(5))) {
    return;
  }

  auto        meas = metrics_collector.create_time_execution_measurer();
  trace_point tp   = ofh_tracer.now();

  auto exp_buffer = buffer_pool.reserve();
  if (!exp_buffer) {
    logger.warning("No buffer is available for receiving an Ethernet packet on the port bound to fd = '{}'", socket_fd);
    return;
  }
  ethernet_rx_buffer_impl buffer    = std::move(*exp_buffer);
  span<uint8_t>           data_span = buffer.storage();
  auto                    nof_bytes = ::recvfrom(socket_fd, data_span.data(), BUFFER_SIZE, 0, nullptr, nullptr);

  if (nof_bytes < 0) {
    logger.warning("Ethernet receiver call to recvfrom failed, fd = '{}'", socket_fd);
    metrics_collector.update_stats(meas.stop());
    return;
  }
  buffer.resize(nof_bytes);

  metrics_collector.update_stats(meas.stop(), nof_bytes);

  notifier->on_new_frame(unique_rx_buffer(std::move(buffer)));
  ofh_tracer << trace_event("ofh_receiver", tp);
}

receiver_metrics_collector* receiver_impl::get_metrics_collector()
{
  return metrics_collector.disabled() ? nullptr : &metrics_collector;
}
