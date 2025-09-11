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

#include "dpdk_ethernet_receiver.h"
#include "srsran/instrumentation/traces/ofh_traces.h"
#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_rx_buffer.h"
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
  void on_new_frame(ether::unique_rx_buffer buffer) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the DPDK Ethernet receiver implementation as a placeholder for the
/// actual frame notifier, which will be later set up through the \ref start() method.
static dummy_frame_notifier dummy_notifier;

dpdk_receiver_impl::dpdk_receiver_impl(task_executor&                     executor_,
                                       std::shared_ptr<dpdk_port_context> port_ctx_,
                                       srslog::basic_logger&              logger_,
                                       bool                               are_metrics_enabled) :
  logger(logger_),
  executor(executor_),
  notifier(&dummy_notifier),
  port_ctx(std::move(port_ctx_)),
  metrics_collector(are_metrics_enabled)
{
  srsran_assert(port_ctx, "Invalid port context");
}

void dpdk_receiver_impl::start(frame_notifier& notifier_)
{
  stop_manager.reset();

  notifier = &notifier_;

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (!executor.defer([this, &p]() {
        // Signal to the start() caller thread that the operation is complete.
        p.set_value();
        receive_loop();
      })) {
    report_error("Unable to start the DPDK ethernet frame receiver on port '{}'", port_ctx->get_port_id());
  }

  // Block waiting for timing executor to start.
  fut.wait();

  logger.info("Started the DPDK ethernet frame receiver on port '{}'", port_ctx->get_port_id());
}

void dpdk_receiver_impl::stop()
{
  logger.info("Requesting stop of the DPDK ethernet frame receiver on port '{}'", port_ctx->get_port_id());
  stop_manager.stop();

  logger.info("Stopped the DPDK ethernet frame receiver on port '{}'", port_ctx->get_port_id());
}

void dpdk_receiver_impl::receive_loop()
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

void dpdk_receiver_impl::receive()
{
  std::array<::rte_mbuf*, MAX_BURST_SIZE> mbufs;

  auto        meas       = metrics_collector.create_time_execution_measurer();
  trace_point dpdk_rx_tp = ofh_tracer.now();

  unsigned num_frames = ::rte_eth_rx_burst(port_ctx->get_dpdk_port_id(), 0, mbufs.data(), MAX_BURST_SIZE);
  if (num_frames == 0) {
    ofh_tracer << instant_trace_event("ofh_receiver_wait_data", instant_trace_event::cpu_scope::thread);
    metrics_collector.update_stats(meas.stop());

    std::this_thread::sleep_for(std::chrono::microseconds(5));
    return;
  }

  if (!metrics_collector.disabled()) {
    uint64_t nof_bytes_received = 0;
    for (auto* mbuf : span<::rte_mbuf*>(mbufs.data(), num_frames)) {
      nof_bytes_received += mbuf->data_len;
    }
    metrics_collector.update_stats(meas.stop(), nof_bytes_received, num_frames);
  }

  for (auto* mbuf : span<::rte_mbuf*>(mbufs.data(), num_frames)) {
    ::rte_vlan_strip(mbuf);
    notifier->on_new_frame(unique_rx_buffer(dpdk_rx_buffer_impl(mbuf)));
  }
  ofh_tracer << trace_event("ofh_dpdk_rx", dpdk_rx_tp);
}

receiver_metrics_collector* dpdk_receiver_impl::get_metrics_collector()
{
  return metrics_collector.disabled() ? nullptr : &metrics_collector;
}
