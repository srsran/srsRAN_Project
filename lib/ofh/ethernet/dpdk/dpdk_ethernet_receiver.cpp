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

#include "dpdk_ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/support/executors/task_executor.h"
#include <rte_ethdev.h>
#include <thread>

using namespace srsran;
using namespace ether;

static constexpr unsigned MAX_BURST_SIZE  = 32;
static constexpr unsigned MAX_BUFFER_SIZE = 9600;

void dpdk_receiver_impl::start()
{
  logger.info("Starting the DPDK ethernet frame receiver");
  executor.defer([this]() { receive_loop(); });
}

void dpdk_receiver_impl::stop()
{
  logger.info("Requesting stop of the DPDK ethernet frame receiver");
  is_stop_requested.store(true, std::memory_order::memory_order_relaxed);
}

void dpdk_receiver_impl::receive_loop()
{
  receive();

  if (is_stop_requested.load(std::memory_order_relaxed)) {
    return;
  }

  executor.defer([this]() { receive_loop(); });
}

void dpdk_receiver_impl::receive()
{
  std::array<::rte_mbuf*, MAX_BURST_SIZE> mbufs;
  unsigned                                num_frames = ::rte_eth_rx_burst(port_id, 0, mbufs.data(), MAX_BURST_SIZE);
  if (num_frames == 0) {
    std::this_thread::sleep_for(std::chrono::microseconds(5));
    return;
  }

  for (auto mbuf : span<::rte_mbuf*>(mbufs.data(), num_frames)) {
    std::array<uint8_t, MAX_BUFFER_SIZE> buffer;

    ::rte_vlan_strip(mbuf);
    ::rte_ether_hdr* eth    = rte_pktmbuf_mtod(mbuf, ::rte_ether_hdr*);
    unsigned         length = mbuf->pkt_len;

    std::memcpy(buffer.data(), eth, length);
    ::rte_pktmbuf_free(mbuf);

    notifier.on_new_frame(span<const uint8_t>(buffer.data(), length));
  }
}

/// Closes an Ethernet port using DPDK.
static void dpdk_eth_close()
{
  unsigned portid;
  RTE_ETH_FOREACH_DEV(portid)
  {
    fmt::print("Closing port {}...", portid);
    int ret = ::rte_eth_dev_stop(portid);
    if (ret != 0) {
      fmt::print("rte_eth_dev_stop: err={}, port={}\n", ret, portid);
    }
    ::rte_eth_dev_close(portid);
    fmt::print(" Done\n");
  }
}

dpdk_receiver_impl::~dpdk_receiver_impl()
{
  dpdk_eth_close();
}
