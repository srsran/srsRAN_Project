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

#include "downlink_processor_multi_executor_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/upper/channel_processors/pdcch/formatters.h"
#include "srsran/phy/upper/channel_processors/pdsch/formatters.h"
#include "srsran/phy/upper/channel_processors/ssb/formatters.h"
#include "srsran/phy/upper/signal_processors/prs/formatters.h"
#include "srsran/phy/upper/signal_processors/signal_processor_formatters.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/rtsan.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

downlink_processor_multi_executor_impl::downlink_processor_multi_executor_impl(
    upper_phy_rg_gateway&                 gateway_,
    std::unique_ptr<pdcch_processor>      pdcch_proc_,
    std::unique_ptr<pdsch_processor>      pdsch_proc_,
    std::unique_ptr<ssb_processor>        ssb_proc_,
    std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc_,
    std::unique_ptr<prs_generator>        prs_gen_,
    task_executor&                        pdcch_executor_,
    task_executor&                        pdsch_executor_,
    task_executor&                        ssb_executor_,
    task_executor&                        csi_rs_executor_,
    task_executor&                        prs_executor_,
    srslog::basic_logger&                 logger_) :
  gateway(gateway_),
  pdcch_proc(std::move(pdcch_proc_)),
  pdsch_proc(std::move(pdsch_proc_)),
  ssb_proc(std::move(ssb_proc_)),
  csi_rs_proc(std::move(csi_rs_proc_)),
  prs_gen(std::move(prs_gen_)),
  pdcch_executor(pdcch_executor_),
  pdsch_executor(pdsch_executor_),
  ssb_executor(ssb_executor_),
  csi_rs_executor(csi_rs_executor_),
  prs_executor(prs_executor_),
  logger(logger_),
  pdsch_notifier(*this)
{
  srsran_assert(pdcch_proc, "Invalid PDCCH processor received.");
  srsran_assert(pdsch_proc, "Invalid PDSCH processor received.");
  srsran_assert(ssb_proc, "Invalid SSB processor received.");
  srsran_assert(csi_rs_proc, "Invalid CSI-RS processor received.");
  srsran_assert(prs_gen, "Invalid PRS generator received.");
}

void downlink_processor_multi_executor_impl::process_pdcch(const pdcch_processor::pdu_t& pdu)
{
  // Notify to the state the creation of a new task.
  state.on_task_creation();

  // Copy the PDU and get the reference for the asynchronous processing.
  pdcch_processor::pdu_t& pdu_ref = pdcch_list.emplace_back(pdu);

  // Try to enqueue the PDU processing task.
  bool enqueued = pdcch_executor.defer([this, &pdu_ref]() noexcept SRSRAN_RTSAN_NONBLOCKING {
    trace_point process_pdcch_tp = l1_dl_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid) {
      pdcch_proc->process(current_grid.get_writer(), pdu_ref);
    }

    l1_dl_tracer << trace_event("process_pdcch", process_pdcch_tp);

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

void downlink_processor_multi_executor_impl::process_pdsch(
    static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data_,
    const pdsch_processor::pdu_t&                                                    pdu)
{
  // Notify to the state the creation of a new task.
  state.on_task_creation();

  // Copy the PDU and data and get the reference for the asynchronous processing.
  pdsch_proc_args& pdsch_args = pdsch_list.emplace_back(pdu, std::move(data_));

  // Try to enqueue the PDU processing task.
  bool enqueued = pdsch_executor.defer([this, &pdsch_args]() mutable noexcept SRSRAN_RTSAN_NONBLOCKING {
    trace_point process_pdsch_tp = l1_dl_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid) {
      pdsch_proc->process(current_grid.get_writer(), pdsch_notifier, std::move(pdsch_args.data), pdsch_args.pdu);

      l1_dl_tracer << trace_event("process_pdsch", process_pdsch_tp);
    } else {
      // Inform about the dropped PDSCH.
      logger.warning(pdsch_args.pdu.slot.sfn(),
                     pdsch_args.pdu.slot.slot_index(),
                     "Resource grid not configured. Ignoring PDSCH {:s}.",
                     pdsch_args.pdu);

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

void downlink_processor_multi_executor_impl::process_ssb(const ssb_processor::pdu_t& pdu)
{
  // Notify to the state the creation of a new task.
  state.on_task_creation();

  // Copy the PDU and get the reference for the asynchronous processing.
  ssb_processor::pdu_t& pdu_ref = ssb_list.emplace_back(pdu);

  // Try to enqueue the PDU processing task.
  bool enqueued = ssb_executor.defer([this, &pdu_ref]() noexcept SRSRAN_RTSAN_NONBLOCKING {
    trace_point process_ssb_tp = l1_dl_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid) {
      ssb_proc->process(current_grid->get_writer(), pdu_ref);
    }

    l1_dl_tracer << trace_event("process_ssb", process_ssb_tp);

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

void downlink_processor_multi_executor_impl::process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config)
{
  // Notify to the state the creation of a new task.
  state.on_task_creation();

  // Copy the configuration and get the reference for the asynchronous processing.
  nzp_csi_rs_generator::config_t& config_ref = nzp_csi_rs_list.emplace_back(config);

  // Try to enqueue the PDU processing task.
  bool enqueued = csi_rs_executor.defer([this, &config_ref]() noexcept SRSRAN_RTSAN_NONBLOCKING {
    trace_point process_nzp_csi_rs_tp = l1_dl_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid) {
      csi_rs_proc->map(current_grid->get_writer(), config_ref);
    }

    l1_dl_tracer << trace_event("process_nzp_csi_rs", process_nzp_csi_rs_tp);

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

void downlink_processor_multi_executor_impl::process_prs(const prs_generator_configuration& config)
{
  // Notify to the state the creation of a new task.
  state.on_task_creation();

  // Copy the configuration and get the reference for the asynchronous processing.
  prs_generator_configuration& config_ref = prs_list.emplace_back(config);

  // Try to enqueue the PDU processing task.
  bool enqueued = prs_executor.defer([this, &config_ref]() noexcept SRSRAN_RTSAN_NONBLOCKING {
    trace_point process_prs_tp = l1_dl_tracer.now();

    // Do not execute if the grid is not available.
    if (current_grid) {
      prs_gen->generate(current_grid->get_writer(), config_ref);
    }

    l1_dl_tracer << trace_event("process_prs", process_prs_tp);

    // Report task completion to FSM.
    on_task_completion();
  });

  // If que task could not be enqueued.
  if (!enqueued) {
    // Report task drop to FSM.
    on_task_completion();

    logger.warning(config.slot.sfn(), config.slot.slot_index(), "Failed to process PRS: {:s}", config);
  }
}

unique_downlink_processor
downlink_processor_multi_executor_impl::configure_resource_grid(const resource_grid_context& context,
                                                                shared_resource_grid         grid)
{
  // Try to reserve the downlink processor.
  if (!state.reserve_on_resource_grid_configure()) {
    // Return an invalid downlink processor if the downlink processor is not available.
    return {};
  }

  report_fatal_error_if_not(!current_grid, "A previously configured resource grid is still in use.");

  current_grid = std::move(grid);
  rg_context   = context;

  // Clear temporary transmission PDUs and configurations.
  pdcch_list.clear();
  pdsch_list.clear();
  ssb_list.clear();
  nzp_csi_rs_list.clear();

  l1_dl_tracer << instant_trace_event("configure_rg", instant_trace_event::cpu_scope::global);

  return unique_downlink_processor(*this);
}

void downlink_processor_multi_executor_impl::finish_processing_pdus()
{
  l1_dl_tracer << instant_trace_event("finish_processing_pdus", instant_trace_event::cpu_scope::thread);

  // Notify that no more PDUs will be enqueued. The grid will be sent right away if the current state allows it.
  if (state.on_finish_requested()) {
    send_resource_grid();
  }
}

void downlink_processor_multi_executor_impl::send_resource_grid()
{
  l1_dl_tracer << instant_trace_event("send_resource_grid", instant_trace_event::cpu_scope::global);

  // Send the resource grid if available.
  if (current_grid.is_valid()) {
    gateway.send(rg_context, std::move(current_grid));
  }

  // Notify the resource grid sent.
  state.on_grid_sent();
}

void downlink_processor_multi_executor_impl::on_task_completion()
{
  // Notify the task completion and send the resource grid if allowed.
  if (state.on_task_completion()) {
    send_resource_grid();
  }
}
