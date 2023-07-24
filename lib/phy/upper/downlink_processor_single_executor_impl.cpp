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

#include "downlink_processor_single_executor_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"
#include "srsran/support/executors/task_executor.h"

using namespace srsran;

downlink_processor_single_executor_impl::downlink_processor_single_executor_impl(
    upper_phy_rg_gateway&                 gateway_,
    std::unique_ptr<pdcch_processor>      pdcch_proc_,
    std::unique_ptr<pdsch_processor>      pdsch_proc_,
    std::unique_ptr<ssb_processor>        ssb_proc_,
    std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc_,
    task_executor&                        executor_) :
  gateway(gateway_),
  current_grid(nullptr),
  pdcch_proc(std::move(pdcch_proc_)),
  pdsch_proc(std::move(pdsch_proc_)),
  ssb_proc(std::move(ssb_proc_)),
  csi_rs_proc(std::move(csi_rs_proc_)),
  executor(executor_)
{
  srsran_assert(pdcch_proc, "Invalid PDCCH processor received.");
  srsran_assert(pdsch_proc, "Invalid PDSCH processor received.");
  srsran_assert(ssb_proc, "Invalid SSB processor received.");
  srsran_assert(csi_rs_proc, "Invalid CSI-RS processor received.");
}

bool downlink_processor_single_executor_impl::process_pdcch(const pdcch_processor::pdu_t& pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      return false;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, pdu]() {
    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      resource_grid_mapper& mapper = current_grid->get_mapper();
      pdcch_proc->process(mapper, pdu);
    }

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();
    return false;
  }
  return true;
}

bool downlink_processor_single_executor_impl::process_pdsch(
    const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
    const pdsch_processor::pdu_t&                                                        pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      return false;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, data, pdu]() {
    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      resource_grid_mapper& mapper = current_grid->get_mapper();
      pdsch_proc->process(mapper, data, pdu);
    }

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();
    return false;
  }
  return true;
}

bool downlink_processor_single_executor_impl::process_ssb(const ssb_processor::pdu_t& pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      return false;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, pdu]() {
    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      ssb_proc->process(current_grid->get_writer(), pdu);
    }

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();
    return false;
  }
  return true;
}

bool downlink_processor_single_executor_impl::process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      return false;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, config]() {
    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      resource_grid_mapper& mapper = current_grid->get_mapper();
      csi_rs_proc->map(mapper, config);
    }

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();
    return false;
  }
  return true;
}

bool downlink_processor_single_executor_impl::configure_resource_grid(const resource_grid_context& context,
                                                                      resource_grid&               grid)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Don't configure the grid if the DL processor is not available.
    if (!state.is_idle()) {
      return false;
    }

    report_fatal_error_if_not(current_grid == nullptr, "A previously configured resource grid is still in use.");

    current_grid = &grid;
    rg_context   = context;

    // update internal state to allow processing PDUs and increase the pending task counter.
    state.on_resource_grid_configured();
    state.on_task_creation();
  }

  // Set the resource grid to zero asynchronously.
  bool enqueued = executor.execute([this]() {
    current_grid->set_all_zero();
    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();
    return false;
  }

  return true;
}

void srsran::downlink_processor_single_executor_impl::finish_processing_pdus()
{
  bool can_send_grid = false;
  {
    std::lock_guard<std::mutex> lock(mutex);
    // No more PDUs will be processed. If there are no pending or running tasks, the grid will be sent right away.
    can_send_grid = state.on_finish_requested();
  }
  // Send the grid if all the PDUs finished to process.
  if (can_send_grid) {
    send_resource_grid();
  }
}

void downlink_processor_single_executor_impl::send_resource_grid()
{
  // Send the resource grid if available.
  if (current_grid != nullptr) {
    gateway.send(rg_context, current_grid->get_reader());
    current_grid = nullptr;
  }

  // Update internal state.
  {
    std::lock_guard<std::mutex> lock(mutex);
    state.on_grid_sent();
  }
}

void downlink_processor_single_executor_impl::on_task_completion()
{
  // Decrement the number of pending PDUs.
  bool can_send_grid = false;
  {
    std::lock_guard<std::mutex> lock(mutex);
    can_send_grid = state.on_task_completion();
  }
  if (can_send_grid) {
    send_resource_grid();
  }
}
