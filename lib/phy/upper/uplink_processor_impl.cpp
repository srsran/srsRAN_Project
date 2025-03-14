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

#include "uplink_processor_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"

using namespace srsran;

/// \brief Returns a PRACH detector slot configuration using the given PRACH buffer context.
static prach_detector::configuration get_prach_dectector_config_from_prach_context(const prach_buffer_context& context)
{
  prach_detector::configuration config;
  config.root_sequence_index   = context.root_sequence_index;
  config.format                = context.format;
  config.restricted_set        = context.restricted_set;
  config.zero_correlation_zone = context.zero_correlation_zone;
  config.start_preamble_index  = context.start_preamble_index;
  config.nof_preamble_indices  = context.nof_preamble_indices;
  config.ra_scs                = to_ra_subcarrier_spacing(context.pusch_scs);
  config.nof_rx_ports          = context.ports.size();
  config.slot                  = context.slot;

  return config;
}

uplink_processor_impl::uplink_processor_impl(std::unique_ptr<prach_detector>  prach_,
                                             std::unique_ptr<pusch_processor> pusch_proc_,
                                             std::unique_ptr<pucch_processor> pucch_proc_,
                                             std::unique_ptr<srs_estimator>   srs_,
                                             task_executor_collection&        task_executors_,
                                             rx_buffer_pool&                  rm_buffer_pool_,
                                             upper_phy_rx_results_notifier&   notifier_,
                                             unsigned                         max_nof_prb,
                                             unsigned                         max_nof_layers) :
  prach(std::move(prach_)),
  pusch_proc(std::move(pusch_proc_)),
  pucch_proc(std::move(pucch_proc_)),
  srs(std::move(srs_)),
  task_executors(task_executors_),
  rm_buffer_pool(rm_buffer_pool_),
  rx_payload_pool(max_nof_prb, max_nof_layers),
  logger(srslog::fetch_basic_logger("PHY", true)),
  notifier(notifier_)
{
  srsran_assert(prach, "A valid PRACH detector must be provided");
  srsran_assert(pusch_proc, "A valid PUSCH processor must be provided");
  srsran_assert(pucch_proc, "A valid PUCCH processor must be provided");
  srsran_assert(srs, "A valid SRS channel estimator must be provided");
}

uplink_slot_processor& uplink_processor_impl::get_slot_processor()
{
  return *this;
}

unique_uplink_pdu_slot_repository uplink_processor_impl::get_pdu_slot_repository(slot_point slot)
{
  // Try to configure a new slot.
  if (!pdu_repository.new_slot()) {
    // Return an invalid repository.
    return {};
  }

  // Reset processor states.
  current_slot         = slot;
  count_pusch_adaptors = 0;
  rx_payload_pool.reset();

  // Create a valid unique PDU slot repository.
  return unique_uplink_pdu_slot_repository(pdu_repository);
}

void uplink_processor_impl::handle_rx_symbol(const shared_resource_grid& grid, unsigned end_symbol_index)
{
  // Extract PDUs for the given number of OFDM symbols.
  const auto pdus = pdu_repository.get_pdus(end_symbol_index);

  // Run rate matching buffer pool only at the first symbol.
  if (end_symbol_index == 0) {
    rm_buffer_pool.run_slot(current_slot);
  }

  // Skip if no PDUs are available.
  if (pdus.empty()) {
    return;
  }

  // Process all the PDUs taken from the repository.
  for (const auto& pdu : pdus) {
    if (const auto* pusch_pdu = std::get_if<uplink_pdu_slot_repository::pusch_pdu>(&pdu)) {
      process_pusch(grid, *pusch_pdu);
    } else if (const auto* pucch_pdu = std::get_if<uplink_pdu_slot_repository::pucch_pdu>(&pdu)) {
      process_pucch(grid, *pucch_pdu);
    } else if (const auto* srs_pdu = std::get_if<uplink_pdu_slot_repository::srs_pdu>(&pdu)) {
      process_srs(grid, *srs_pdu);
    }
  }
}

void uplink_processor_impl::process_prach(const prach_buffer& buffer, const prach_buffer_context& context_)
{
  bool success = task_executors.prach_executor.execute([this, &buffer, context_]() mutable {
    trace_point tp = l1_tracer.now();

    ul_prach_results ul_results;
    ul_results.context = context_;
    ul_results.result  = prach->detect(buffer, get_prach_dectector_config_from_prach_context(context_));

    // Notify the PRACH results.
    notifier.on_new_prach_results(ul_results);

    l1_tracer << trace_event("process_prach", tp);
  });

  if (!success) {
    logger.warning(current_slot.sfn(), current_slot.slot_index(), "Failed to execute PRACH. Ignoring detection.");
  }
}

void uplink_processor_impl::process_pusch(const shared_resource_grid&                  grid,
                                          const uplink_pdu_slot_repository::pusch_pdu& pdu)
{
  const pusch_processor::pdu_t& proc_pdu = pdu.pdu;

  // Temporal sanity check as PUSCH is only supported for data. Remove the check when the UCI is supported for PUSCH.
  srsran_assert(proc_pdu.codeword.has_value(),
                "PUSCH PDU doesn't contain data. Currently, that mode is not supported.");

  // Create buffer identifier.
  trx_buffer_identifier id(proc_pdu.rnti, pdu.harq_id);

  // Determine the number of codeblocks from the TBS and base graph.
  unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(pdu.tb_size.to_bits(), proc_pdu.codeword->ldpc_base_graph);

  // Extract new data flag.
  bool new_data = proc_pdu.codeword->new_data;

  // Reserve receive buffer.
  unique_rx_buffer rm_buffer = rm_buffer_pool.reserve(current_slot, id, nof_codeblocks, new_data);

  // Skip processing if the buffer is not valid. The pool shall log the context and the reason of the failure.
  if (!rm_buffer) {
    notify_discard_pusch(pdu);
    return;
  }

  // Retrieves transport block data and starts PUSCH processing.
  auto data = rx_payload_pool.acquire_payload_buffer(pdu.tb_size);
  if (data.empty()) {
    logger.warning(pdu.pdu.slot.sfn(),
                   pdu.pdu.slot.slot_index(),
                   "UL rnti={} h_id={}: insufficient available payload data in the buffer pool.",
                   pdu.pdu.rnti,
                   pdu.harq_id);
    notify_discard_pusch(pdu);
    return;
  }

  // Try to enqueue asynchronous processing.
  bool success = task_executors.pusch_executor.execute(
      [this, grid2 = grid.copy(), data, rm_buffer2 = std::move(rm_buffer), &pdu]() mutable {
        // Select and configure notifier adaptor.
        // Assume that count_pusch_adaptors will not exceed MAX_PUSCH_PDUS_PER_SLOT.
        unsigned                         notifier_adaptor_id = count_pusch_adaptors.fetch_add(1);
        pusch_processor_result_notifier& processor_notifier  = pusch_adaptors[notifier_adaptor_id].configure(
            notifier, to_rnti(pdu.pdu.rnti), pdu.pdu.slot, to_harq_id(pdu.harq_id), data, [this]() {
              pdu_repository.on_finish_processing_pdu();
            });

        trace_point tp = l1_tracer.now();

        pusch_proc->process(data, std::move(rm_buffer2), processor_notifier, grid2.get_reader(), pdu.pdu);

        l1_tracer << trace_event("process_pusch", tp);
      });

  // Report the execution failure.
  if (!success) {
    logger.warning(pdu.pdu.slot.sfn(), pdu.pdu.slot.slot_index(), "Failed to execute PUSCH. Ignoring processing.");
    notify_discard_pusch(pdu);
  }
}

void uplink_processor_impl::process_pucch(const shared_resource_grid&                  grid,
                                          const uplink_pdu_slot_repository::pucch_pdu& pdu)
{
  bool success = task_executors.pucch_executor.execute([this, grid2 = grid.copy(), &pdu]() mutable {
    trace_point tp = l1_tracer.now();

    pucch_processor_result proc_result;
    // Process the PUCCH.
    switch (pdu.context.format) {
      case pucch_format::FORMAT_0: {
        const auto& format0 = std::get<pucch_processor::format0_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid2.get_reader(), format0);
        l1_tracer << trace_event("pucch0", tp);
      } break;
      case pucch_format::FORMAT_1: {
        const auto& format1 = std::get<pucch_processor::format1_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid2.get_reader(), format1);
        l1_tracer << trace_event("pucch1", tp);
      } break;
      case pucch_format::FORMAT_2: {
        const auto& format2 = std::get<pucch_processor::format2_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid2.get_reader(), format2);
        l1_tracer << trace_event("pucch2", tp);
      } break;
      case pucch_format::FORMAT_3: {
        const auto& format3 = std::get<pucch_processor::format3_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid2.get_reader(), format3);
        l1_tracer << trace_event("pucch3", tp);
      } break;
      case pucch_format::FORMAT_4: {
        const auto& format4 = std::get<pucch_processor::format4_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid2.get_reader(), format4);
        l1_tracer << trace_event("pucch4", tp);
      } break;
      default:
        srsran_assert(0, "Invalid PUCCH format={}", fmt::underlying(pdu.context.format));
    }

    // Write the results.
    ul_pucch_results result;
    result.context          = pdu.context;
    result.processor_result = proc_result;

    // Notify the PUCCH results.
    notifier.on_new_pucch_results(result);
    pdu_repository.on_finish_processing_pdu();
  });

  // Report failed execution.
  if (!success) {
    logger.warning(
        pdu.context.slot.sfn(), pdu.context.slot.slot_index(), "Failed to execute PUCCH. Ignoring processing.");
    notify_discard_pucch(pdu);
  }
}

void uplink_processor_impl::process_srs(const shared_resource_grid&                grid,
                                        const uplink_pdu_slot_repository::srs_pdu& pdu)
{
  bool success = task_executors.srs_executor.execute([this, grid2 = grid.copy(), &pdu]() {
    trace_point tp = l1_tracer.now();

    ul_srs_results result;
    result.context          = pdu.context;
    result.processor_result = srs->estimate(grid2.get_reader(), pdu.config);

    l1_tracer << trace_event("process_srs", tp);

    notifier.on_new_srs_results(result);
    pdu_repository.on_finish_processing_pdu();
  });

  if (!success) {
    logger.warning(
        pdu.context.slot.sfn(), pdu.context.slot.slot_index(), "Failed to execute SRS. Ignoring processing.");
    pdu_repository.on_finish_processing_pdu();
  }
}

void uplink_processor_impl::notify_discard_pusch(const uplink_pdu_slot_repository::pusch_pdu& pdu)
{
  // Report data-related discarded result if shared channel data is present.
  if (pdu.pdu.codeword.has_value()) {
    ul_pusch_results_data discarded_results =
        ul_pusch_results_data::create_discarded(to_rnti(pdu.pdu.rnti), pdu.pdu.slot, pdu.harq_id);
    notifier.on_new_pusch_results_data(discarded_results);
  }

  // Report control-related discarded result if HARQ-ACK feedback is present.
  if (pdu.pdu.uci.nof_harq_ack > 0) {
    ul_pusch_results_control discarded_results =
        ul_pusch_results_control::create_discarded(to_rnti(pdu.pdu.rnti), pdu.pdu.slot, pdu.pdu.uci.nof_harq_ack);
    notifier.on_new_pusch_results_control(discarded_results);
  }

  pdu_repository.on_finish_processing_pdu();
}

void uplink_processor_impl::notify_discard_pucch(const uplink_pdu_slot_repository::pucch_pdu& pdu)
{
  // Select the number of HARQ-ACK feedback number of bits.
  const unsigned nof_harq_ack = std::visit([](const auto& config) { return config.nof_harq_ack; }, pdu.config);

  // Report control-related discarded result if HARQ-ACK feedback is present.
  if (nof_harq_ack > 0) {
    ul_pucch_results discarded_results = ul_pucch_results::create_discarded(pdu.context, nof_harq_ack);
    notifier.on_new_pucch_results(discarded_results);
  }
  pdu_repository.on_finish_processing_pdu();
}

void uplink_processor_impl::discard_slot()
{
  logger.warning(current_slot.sfn(), current_slot.slot_index(), "Discarded uplink slot. Ignoring processing.");

  // Iterate all possible symbols.
  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    span<const uplink_slot_pdu_entry> pdus = pdu_repository.get_pdus(i_symbol);

    // Notify all the discarded PDUs.
    for (const auto& pdu : pdus) {
      if (const auto* pusch_pdu = std::get_if<uplink_pdu_slot_repository::pusch_pdu>(&pdu)) {
        notify_discard_pusch(*pusch_pdu);
      } else if (const auto* pucch_pdu = std::get_if<uplink_pdu_slot_repository::pucch_pdu>(&pdu)) {
        notify_discard_pucch(*pucch_pdu);
      }
    }
  }
}
