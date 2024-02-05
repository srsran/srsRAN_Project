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

#pragma once

#include "srsran/gateways/udp_network_gateway_factory.h"
#include <arpa/inet.h>

namespace srsran {
class dummy_network_gateway_data_notifier_with_src_addr : public network_gateway_data_notifier_with_src_addr
{
public:
  dummy_network_gateway_data_notifier_with_src_addr(unsigned slow_inter_rx_us_, uint64_t max_pdus_) :
    slow_inter_rx_us(slow_inter_rx_us_), max_pdus(max_pdus_)
  {
  }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    if (n_pdus > max_pdus) {
      // test is finished stop counting
      return;
    }
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
      if (duration.count() > slow_inter_rx_us) {
        fmt::print("Long inter Rx interval t={}us at n_pdus={}\n", duration.count(), n_pdus);
      }
    } else {
      first   = false;
      t_start = t_now;
    }

    t_last = t_now;
    if (n_pdus == max_pdus) {
      t_end = std::chrono::high_resolution_clock::now();
    }
  }

  uint64_t get_rx_bytes() const { return rx_bytes; }
  uint64_t get_n_pdus() const { return n_pdus; }

  std::chrono::microseconds get_t_min() { return t_min; }
  std::chrono::microseconds get_t_max() { return t_max; }
  std::chrono::microseconds get_t_sum() { return t_sum; }
  std::chrono::microseconds get_t_rx()
  {
    if (n_pdus >= max_pdus) {
      return std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
    }
    return std::chrono::duration_cast<std::chrono::microseconds>(t_last - t_start);
  }

private:
  unsigned slow_inter_rx_us;

  uint64_t rx_bytes = 0;
  uint64_t n_pdus   = 0;
  uint64_t max_pdus;

  std::chrono::high_resolution_clock::time_point t_last = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds                      t_min  = std::chrono::microseconds::max();
  std::chrono::microseconds                      t_max  = std::chrono::microseconds::min();
  std::chrono::microseconds                      t_sum  = std::chrono::microseconds::zero();

  std::chrono::high_resolution_clock::time_point t_start;
  std::chrono::high_resolution_clock::time_point t_end;
};

inline byte_buffer make_tx_byte_buffer(uint32_t length)
{
  byte_buffer pdu{};
  for (uint32_t i = 0; i < length; ++i) {
    if (not pdu.append((uint8_t)i)) {
      pdu.clear();
      break;
    }
  }
  return pdu;
}

inline sockaddr_storage to_sockaddr_storage(const std::string& dest_addr, uint16_t port)
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

} // namespace srsran
