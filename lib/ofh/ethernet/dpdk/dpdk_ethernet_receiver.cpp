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

#include "dpdk_ethernet_receiver.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/support/executors/task_executor.h"
#include <future>
#include <rte_ethdev.h>
#include <thread>

using namespace srsran;
using namespace ether;

namespace {

class dummy_frame_notifier : public frame_notifier
{
  // See interface for documentation.
  void on_new_frame(span<const uint8_t> payload) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the DPDK Ethernet receiver implementation as a placeholder for the
/// actual frame notifier, which will be later set up through the \ref start() method.
static dummy_frame_notifier dummy_notifier;

dpdk_receiver_impl::dpdk_receiver_impl(task_executor&                     executor_,
                                       std::shared_ptr<dpdk_port_context> port_ctx_ptr_,
                                       srslog::basic_logger&              logger_) :
  logger(logger_),
  executor(executor_),
  notifier(dummy_notifier),
  port_ctx_ptr(std::move(port_ctx_ptr_)),
  port_ctx(*port_ctx_ptr)
{
  srsran_assert(port_ctx_ptr, "Invalid port context");
}

void dpdk_receiver_impl::start(frame_notifier& notifier_)
{
  notifier = std::ref(notifier_);

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (!executor.defer([this, &p]() {
        rx_status.store(receiver_status::running, std::memory_order_relaxed);
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

void dpdk_receiver_impl::stop()
{
  logger.info("Requesting stop of the DPDK ethernet frame receiver");
  rx_status.store(receiver_status::stop_requested, std::memory_order_relaxed);

  // Wait for the receiver thread to stop.
  while (rx_status.load(std::memory_order_acquire) != receiver_status::stopped) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  logger.info("Stopped the DPDK ethernet frame receiver");
}

void dpdk_receiver_impl::receive_loop()
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

void dpdk_receiver_impl::receive()
{
  std::array<::rte_mbuf*, MAX_BURST_SIZE> mbufs;
  unsigned num_frames = ::rte_eth_rx_burst(port_ctx.get_port_id(), 0, mbufs.data(), MAX_BURST_SIZE);
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

    notifier.get().on_new_frame(span<const uint8_t>(buffer.data(), length));
  }
}
