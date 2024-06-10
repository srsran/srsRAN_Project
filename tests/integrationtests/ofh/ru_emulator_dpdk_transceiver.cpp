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

#include "../../../lib/ofh/ethernet/dpdk/dpdk_ethernet_rx_buffer_impl.h"
#include "ru_emulator_transceiver.h"
#include <chrono>
#include <future>
#include <rte_ethdev.h>
#include <thread>

using namespace srsran;
using namespace ether;

void dpdk_transceiver::start(frame_notifier& notifier_)
{
  notifier = &notifier_;

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (!executor.defer([this, &p]() {
        trx_status.store(status::running, std::memory_order_relaxed);
        // Signal start() caller thread that the operation is complete.
        p.set_value();
        receive_loop();
      })) {
    report_error("Unable to start the DPDK ethernet frame receiver");
  }

  // Block waiting for timing executor to start.
  fut.wait();

  logger.info("Started the DPDK ethernet frame receiver");
}

void dpdk_transceiver::stop()
{
  logger.info("Requesting stop of the DPDK ethernet frame receiver");
  trx_status.store(status::stop_requested, std::memory_order_relaxed);

  // Wait for the receiver thread to stop.
  while (trx_status.load(std::memory_order_acquire) != status::stopped) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  logger.info("Stopped the DPDK ethernet frame receiver");
}

void dpdk_transceiver::receive_loop()
{
  if (trx_status.load(std::memory_order_relaxed) == status::stop_requested) {
    trx_status.store(status::stopped, std::memory_order_release);
    return;
  }

  receive();

  // Retry the task deferring when it fails.
  while (!executor.defer([this]() { receive_loop(); })) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void dpdk_transceiver::receive()
{
  std::array<::rte_mbuf*, MAX_BURST_SIZE> mbufs;
  unsigned num_frames = ::rte_eth_rx_burst(port_ctx.get_port_id(), 0, mbufs.data(), MAX_BURST_SIZE);
  if (num_frames == 0) {
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    return;
  }

  for (auto mbuf : span<::rte_mbuf*>(mbufs.data(), num_frames)) {
    ::rte_vlan_strip(mbuf);
    dpdk_rx_buffer_impl buffer(mbuf);
    notifier->on_new_frame(std::move(buffer));
  }
}

void dpdk_transceiver::send(span<span<const uint8_t>> frames)
{
  // Receiving a frame burst larger than MAX_BURST_SIZE requires making several Tx bursts.
  for (unsigned offset = 0; offset < frames.size();) {
    auto frame_burst = frames.subspan(offset, std::min<unsigned>(MAX_BURST_SIZE, frames.size() - offset));
    offset += frames.size();

    static_vector<::rte_mbuf*, MAX_BURST_SIZE> mbufs(frame_burst.size());
    if (::rte_pktmbuf_alloc_bulk(port_ctx.get_mempool(), mbufs.data(), frame_burst.size()) < 0) {
      logger.warning("Not enough entries in the mempool to send '{}' frames in the DPDK Ethernet transmitter",
                     frame_burst.size());
      return;
    }

    for (unsigned idx = 0, end = frame_burst.size(); idx != end; ++idx) {
      const auto  frame = frame_burst[idx];
      ::rte_mbuf* mbuf  = mbufs[idx];

      if (::rte_pktmbuf_append(mbuf, frame.size()) == nullptr) {
        ::rte_pktmbuf_free(mbuf);
        logger.warning("Unable to append '{}' bytes to the allocated mbuf in the DPDK Ethernet transmitter",
                       frame.size());
        ::rte_pktmbuf_free_bulk(mbufs.data(), mbufs.size());
        return;
      }
      mbuf->data_len = frame.size();
      mbuf->pkt_len  = frame.size();

      uint8_t* data = rte_pktmbuf_mtod(mbuf, uint8_t*);
      std::memcpy(data, frame.data(), frame.size());
    }

    unsigned nof_sent_packets = ::rte_eth_tx_burst(port_ctx.get_port_id(), 0, mbufs.data(), mbufs.size());

    if (SRSRAN_UNLIKELY(nof_sent_packets < mbufs.size())) {
      logger.warning("DPDK dropped '{}' packets out of a total of '{}' in the tx burst",
                     mbufs.size() - nof_sent_packets,
                     mbufs.size());
      for (unsigned buf_idx = nof_sent_packets, last_idx = mbufs.size(); buf_idx != last_idx; ++buf_idx) {
        ::rte_pktmbuf_free(mbufs[buf_idx]);
      }
    }
  }
}
