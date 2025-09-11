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

#include "ethernet_transmitter_impl.h"
#include "srsran/support/error_handling.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <net/if.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace srsran;
using namespace ether;

transmitter_impl::transmitter_impl(const transmitter_config& config, srslog::basic_logger& logger_) :
  logger(logger_), metrics_collector(config.are_metrics_enabled)
{
  socket_fd = ::socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, IPPROTO_RAW);
  if (socket_fd < 0) {
    report_error("Unable to open raw socket for Ethernet transmitter: {}", ::strerror(errno));
  }

  if (config.interface.size() > (IFNAMSIZ - 1)) {
    report_error("The Ethernet transmitter interface name '{}' exceeds the maximum allowed length");
  }

  ::ifreq if_idx = {};
  ::strncpy(if_idx.ifr_name, config.interface.c_str(), IFNAMSIZ - 1);

  // Set requested MTU size.
  if_idx.ifr_mtu = config.mtu_size.value();
  if (::ioctl(socket_fd, SIOCSIFMTU, &if_idx) < 0) {
    // Get the MTU size of the NIC.
    int current_mtu = -1;
    if (::ioctl(socket_fd, SIOCGIFMTU, &if_idx) < 0) {
      logger.warning("Could not check MTU of the NIC interface '{}' in the Ethernet transmitter", config.interface);
    } else {
      current_mtu = if_idx.ifr_mtu;
    }
    report_error(
        "Unable to set MTU size to '{}' bytes for NIC interface '{}' in the Ethernet transmitter, current MTU size "
        "set to '{}' bytes",
        config.mtu_size,
        config.interface,
        current_mtu);
  }

  // Get the index of the NIC.
  if (::ioctl(socket_fd, SIOCGIFINDEX, &if_idx) < 0) {
    report_error("Unable to get index for NIC interface in the Ethernet transmitter");
  }

  // Prepare the socket address used by sendto.
  socket_address             = {};
  socket_address.sll_ifindex = if_idx.ifr_ifindex;
  socket_address.sll_halen   = ETH_ALEN;
  std::copy(std::begin(config.mac_dst_address), std::end(config.mac_dst_address), std::begin(socket_address.sll_addr));

  logger.info("Opened successfully the NIC interface '{}' (fd = '{}') used by the Ethernet transmitter",
              config.interface,
              socket_fd);
}

transmitter_impl::~transmitter_impl()
{
  ::close(socket_fd);
}

void transmitter_impl::send(span<span<const uint8_t>> frames)
{
  for (auto frame : frames) {
    auto meas = metrics_collector.create_time_execution_measurer();

    int ret = ::sendto(socket_fd,
                       frame.data(),
                       frame.size(),
                       0,
                       reinterpret_cast<::sockaddr*>(&socket_address),
                       sizeof(socket_address));

    if (ret < 0) {
      logger.warning("Ethernet transmitter with fd = '{}' could not transmit '{}' bytes, consider tuning "
                     "the NIC system settings to obtain higher performance or use DPDK",
                     socket_fd,
                     frame.size());

      metrics_collector.update_stats(meas.stop());
      continue;
    }
    metrics_collector.update_stats(meas.stop(), frame.size());
  }
}

transmitter_metrics_collector* transmitter_impl::get_metrics_collector()
{
  return metrics_collector.disabled() ? nullptr : &metrics_collector;
}
