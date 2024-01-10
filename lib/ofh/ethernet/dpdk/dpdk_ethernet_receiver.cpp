/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

static constexpr unsigned MAX_BURST_SIZE  = 32;
static constexpr unsigned MAX_BUFFER_SIZE = 9600;

void dpdk_receiver_impl::start()
{
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
  receive();

  if (rx_status.load(std::memory_order_relaxed) == receiver_status::stop_requested) {
    rx_status.store(receiver_status::stopped, std::memory_order_release);
    return;
  }

  // Retry the task deferring when it fails.
  while (!executor.defer([this]() { receive_loop(); })) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
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
