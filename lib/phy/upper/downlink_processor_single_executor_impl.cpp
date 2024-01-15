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

#include "downlink_processor_single_executor_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/signal_processors/signal_processor_formatters.h"
#include "srsran/phy/upper/unique_tx_buffer.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"
#include "srsran/support/executors/task_executor.h"

using namespace srsran;

downlink_processor_single_executor_impl::downlink_processor_single_executor_impl(
    upper_phy_rg_gateway&                 gateway_,
    std::unique_ptr<pdcch_processor>      pdcch_proc_,
    std::unique_ptr<pdsch_processor>      pdsch_proc_,
    std::unique_ptr<ssb_processor>        ssb_proc_,
    std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc_,
    task_executor&                        executor_,
    srslog::basic_logger&                 logger_) :
  gateway(gateway_),
  current_grid(nullptr),
  pdcch_proc(std::move(pdcch_proc_)),
  pdsch_proc(std::move(pdsch_proc_)),
  ssb_proc(std::move(ssb_proc_)),
  csi_rs_proc(std::move(csi_rs_proc_)),
  executor(executor_),
  logger(logger_),
  pdsch_notifier(*this)
{
  srsran_assert(pdcch_proc, "Invalid PDCCH processor received.");
  srsran_assert(pdsch_proc, "Invalid PDSCH processor received.");
  srsran_assert(ssb_proc, "Invalid SSB processor received.");
  srsran_assert(csi_rs_proc, "Invalid CSI-RS processor received.");
}

void downlink_processor_single_executor_impl::process_pdcch(const pdcch_processor::pdu_t& pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      logger.warning(
          pdu.slot.sfn(), pdu.slot.slot_index(), "Invalid downlink processor state. Ignoring PDCCH: {:s}", pdu);
      return;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, pdu]() {
    trace_point process_pdcch_tp = l1_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      resource_grid_mapper& mapper = current_grid->get_mapper();
      pdcch_proc->process(mapper, pdu);
    }

    l1_tracer << trace_event("process_pdcch", process_pdcch_tp);

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();

    logger.warning(pdu.slot.sfn(), pdu.slot.slot_index(), "Failed to process PDCCH: {:s}", pdu);
  }
}

void downlink_processor_single_executor_impl::process_pdsch(
    unique_tx_buffer                                                                     rm_buffer,
    const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
    const pdsch_processor::pdu_t&                                                        pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      logger.warning(
          pdu.slot.sfn(), pdu.slot.slot_index(), "Invalid downlink processor state. Ignoring PDSCH: {:s}", pdu);
      return;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, sb = std::move(rm_buffer), data, pdu]() mutable {
    trace_point process_pdsch_tp = l1_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      resource_grid_mapper& mapper = current_grid->get_mapper();
      pdsch_proc->process(mapper, std::move(sb), pdsch_notifier, data, pdu);

      l1_tracer << trace_event("process_pdsch", process_pdsch_tp);
    } else {
      // Inform about the dropped PDSCH.
      logger.warning(pdu.slot.sfn(), pdu.slot.slot_index(), "Resource grid not configured. Ignoring PDSCH {:s}.", pdu);

      // Report task drop to FSM.
      on_task_completion();
    }
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();

    logger.warning(pdu.slot.sfn(), pdu.slot.slot_index(), "Failed to process PDSCH: {:s}", pdu);
  }
}

void downlink_processor_single_executor_impl::process_ssb(const ssb_processor::pdu_t& pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      logger.warning(
          pdu.slot.sfn(), pdu.slot.slot_index(), "Invalid downlink processor state. Ignoring SSB: {:s}", pdu);
      return;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, pdu]() {
    trace_point process_ssb_tp = l1_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      ssb_proc->process(current_grid->get_writer(), pdu);
    }

    l1_tracer << trace_event("process_ssb", process_ssb_tp);

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();

    logger.warning(pdu.slot.sfn(), pdu.slot.slot_index(), "Failed to process SSB: {:s}", pdu);
  }
}

void downlink_processor_single_executor_impl::process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config)
{
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Do not enqueue the task if the DL processor is not accepting PDUs.
    if (!state.is_processing()) {
      logger.warning(config.slot.sfn(),
                     config.slot.slot_index(),
                     "Invalid downlink processor state. Ignoring NZP-CSI-RS: {:s}",
                     config);
      return;
    }

    // Increase the pending task counter.
    state.on_task_creation();
  }

  // Try to enqueue the PDU processing task.
  bool enqueued = executor.execute([this, config]() {
    trace_point process_nzp_csi_rs_tp = l1_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid != nullptr) {
      resource_grid_mapper& mapper = current_grid->get_mapper();
      csi_rs_proc->map(mapper, config);
    }

    l1_tracer << trace_event("process_nzp_csi_rs", process_nzp_csi_rs_tp);

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();

    logger.warning(config.slot.sfn(), config.slot.slot_index(), "Failed to process NZP-CSI-RS: {:s}", config);
  }
}

bool downlink_processor_single_executor_impl::configure_resource_grid(const resource_grid_context& context,
                                                                      resource_grid&               grid)
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

  l1_tracer << instant_trace_event("configure_rg", instant_trace_event::cpu_scope::thread);

  return true;
}

void srsran::downlink_processor_single_executor_impl::finish_processing_pdus()
{
  l1_tracer << instant_trace_event("finish_processing_pdus", instant_trace_event::cpu_scope::global);

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
  l1_tracer << instant_trace_event("send_resource_grid", instant_trace_event::cpu_scope::global);

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
