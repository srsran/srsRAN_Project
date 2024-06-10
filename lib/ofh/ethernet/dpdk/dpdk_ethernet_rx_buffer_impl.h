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

#include "srsran/ofh/ethernet/ethernet_unique_buffer.h"
#include "srsran/support/srsran_assert.h"
#include <rte_ethdev.h>

namespace srsran {
namespace ether {

/// Receive buffer wrapper dedicated for DPDK Ethernet receiver.
class dpdk_rx_buffer_impl : public rx_buffer
{
public:
  explicit dpdk_rx_buffer_impl(::rte_mbuf* mbuf_) : mbuf(mbuf_)
  {
    srsran_assert(mbuf, "Invalid DPDK mbuf was passed to dpdk_rx_buffer_impl");
  }

  /// Copy constructor is deleted.
  dpdk_rx_buffer_impl(dpdk_rx_buffer_impl& /**/) = delete;

  /// Move constructor.
  dpdk_rx_buffer_impl(dpdk_rx_buffer_impl&& other) noexcept
  {
    mbuf       = other.mbuf;
    other.mbuf = nullptr;
  };

  /// Move assigment operator.
  dpdk_rx_buffer_impl& operator=(dpdk_rx_buffer_impl&& other) noexcept
  {
    // Free stored mbuf.
    ::rte_pktmbuf_free(mbuf);

    mbuf       = other.mbuf;
    other.mbuf = nullptr;
    return *this;
  }

  ~dpdk_rx_buffer_impl() { ::rte_pktmbuf_free(mbuf); }

  // See interface for documentation.
  span<const uint8_t> data() const override
  {
    srsran_assert(mbuf != nullptr, "Invalid dpdk_rx_buffer_impl accessed");

    ::uint8_t* eth_data = rte_pktmbuf_mtod(mbuf, uint8_t*);
    return span<const uint8_t>(eth_data, mbuf->pkt_len);
  }

private:
  ::rte_mbuf* mbuf;
};

} // namespace ether
} // namespace srsran
