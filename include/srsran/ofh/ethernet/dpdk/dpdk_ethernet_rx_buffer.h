/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  /// Move assignment operator.
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
