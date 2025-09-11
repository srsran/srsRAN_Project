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
#include "srsran/adt/scope_exit.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"
#include "srsran/support/rtsan.h"

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
                                             std::unique_ptr<resource_grid>   grid_,
                                             task_executor_collection&        task_executors_,
                                             rx_buffer_pool&                  rm_buffer_pool_,
                                             upper_phy_rx_results_notifier&   notifier_,
                                             unsigned                         max_nof_prb,
                                             unsigned                         max_nof_layers) :
  grid_ref_counter(get_grid_ref_counter()),
  pdu_repository(*grid_, grid_ref_counter, state_machine),
  prach(std::move(prach_)),
  pusch_proc(std::move(pusch_proc_)),
  pucch_proc(std::move(pucch_proc_)),
  srs(std::move(srs_)),
  grid(std::move(grid_)),
  task_executors(task_executors_),
  dummy(*this),
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

uplink_slot_processor& uplink_processor_impl::get_slot_processor(slot_point slot)
{
  // If the slot configured in the state machine does not match the given slot, give the dummy instance.
  if (!state_machine.is_slot_valid(slot)) {
    return dummy;
  }

  return *this;
}

void uplink_processor_impl::stop()
{
  state_machine.stop();
}

unique_uplink_pdu_slot_repository uplink_processor_impl::get_pdu_slot_repository(slot_point slot)
{
  // It is not possible to configure a new slot if the resource grid is still present in a scope.
  if (grid_ref_counter != 0) {
    return {};
  }

  // Discard current slot if possible.
  discard_slot();

  // Try to configure a new slot.
  if (!state_machine.start_new_slot(slot)) {
    // Return an invalid repository.
    return {};
  }

  // Reset processor states.
  current_slot          = slot;
  count_pusch_adaptors  = 0;
  nof_processed_symbols = 0;
  rx_payload_pool.reset();
  pdu_repository.clear_queues();

  // Create a valid unique PDU slot repository.
  return unique_uplink_pdu_slot_repository(pdu_repository);
}

void uplink_processor_impl::handle_rx_symbol(unsigned end_symbol_index, bool is_valid)
{
  // Try locking the slot processor. This prevents that the processor handle symbols and discards from different
  // threads concurrently.
  if (!state_machine.start_handle_rx_symbol()) {
    return;
  }

  // Unlock the slot processor when returning from this method.
  auto execute_on_exit = make_scope_exit([this]() { state_machine.finish_handle_rx_symbol(); });

  // Verify that the symbol index is in increasing order.
  if (end_symbol_index < nof_processed_symbols) {
    logger.warning(current_slot.sfn(),
                   current_slot.slot_index(),
                   "Unexpected symbol index {} is back in time, expected {}.",
                   end_symbol_index,
                   nof_processed_symbols);
    return;
  }

  // Run rate matching buffer pool only at the first symbol.
  if (nof_processed_symbols == 0) {
    rm_buffer_pool.run_slot(current_slot);
  }

  // If the OFDM symbol is not valid, discard all PDUs for the rest of the slot.
  if (!is_valid) {
    // Iterate all symbols that have not been processed yet. As the processor might be executing asynchronous all
    // discarded PDUs must call state_machine.on_create_pdu_task and state_machine.on_finish_processing_pdu for managing
    // the state machine correctly.
    for (unsigned i_symbol = nof_processed_symbols; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
      for (const auto& pdu : pdu_repository.get_pucch_pdus(i_symbol)) {
        if (state_machine.on_create_pdu_task()) {
          notify_discard_pucch(pdu);
        }
      }

      for (const auto& collection : pdu_repository.get_pucch_f1_repository(i_symbol)) {
        if (state_machine.on_create_pdu_task()) {
          notify_discard_pucch(collection);
        }
      }

      for (const auto& pdu : pdu_repository.get_pusch_pdus(i_symbol)) {
        if (state_machine.on_create_pdu_task()) {
          notify_discard_pusch(pdu);
        }
      }

      for ([[maybe_unused]] const auto& pdu : pdu_repository.get_srs_pdus(i_symbol)) {
        if (state_machine.on_create_pdu_task()) {
          state_machine.on_finish_processing_pdu();
        }
      }
    }

    return;
  }

  for (unsigned end_processed_symbol = std::min(end_symbol_index + 1, MAX_NSYMB_PER_SLOT);
       nof_processed_symbols != end_processed_symbol;
       ++nof_processed_symbols) {
    for (const auto& pdu : pdu_repository.get_pucch_pdus(nof_processed_symbols)) {
      process_pucch(pdu);
    }

    for (const auto& collection : pdu_repository.get_pucch_f1_repository(nof_processed_symbols)) {
      process_pucch_f1(collection);
    }

    for (const auto& pdu : pdu_repository.get_pusch_pdus(nof_processed_symbols)) {
      process_pusch(pdu);
    }

    for (const auto& pdu : pdu_repository.get_srs_pdus(nof_processed_symbols)) {
      process_srs(pdu);
    }
  }
}

void uplink_processor_impl::process_prach(const prach_buffer& buffer, const prach_buffer_context& context_)
{
  // Notify the creation of the PRACH detection task.
  if (!state_machine.on_prach_detection()) {
    return;
  }

  bool success = task_executors.prach_executor.execute([this, &buffer, context_]() noexcept SRSRAN_RTSAN_NONBLOCKING {
    trace_point tp = l1_ul_tracer.now();

    ul_prach_results ul_results;
    ul_results.context = context_;
    ul_results.result  = prach->detect(buffer, get_prach_dectector_config_from_prach_context(context_));

    // Notify the PRACH results.
    notifier.on_new_prach_results(ul_results);

    l1_ul_tracer << trace_event("process_prach", tp);

    // Notify the end of the PRACH detection.
    state_machine.on_end_prach_detection();
  });

  if (!success) {
    logger.warning(current_slot.sfn(), current_slot.slot_index(), "Failed to execute PRACH. Ignoring detection.");
    state_machine.on_end_prach_detection();
  }
}

void uplink_processor_impl::process_pusch(const uplink_pdu_slot_repository::pusch_pdu& pdu)
{
  // Notify the creation of the execution task.
  if (!state_machine.on_create_pdu_task()) {
    return;
  }

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
                   "UL rnti={} h_id={}: insufficient available payload data in the buffer pool for TB of size {}",
                   pdu.pdu.rnti,
                   pdu.harq_id,
                   pdu.tb_size);
    notify_discard_pusch(pdu);
    return;
  }

  // Try to enqueue asynchronous processing.
  bool success = task_executors.pusch_executor.defer([this, data, rm_buffer2 = std::move(rm_buffer), &pdu]() mutable {
    // Select and configure notifier adaptor.
    // Assume that count_pusch_adaptors will not exceed MAX_PUSCH_PDUS_PER_SLOT.
    unsigned                         notifier_adaptor_id = count_pusch_adaptors.fetch_add(1);
    pusch_processor_result_notifier& processor_notifier  = pusch_adaptors[notifier_adaptor_id].configure(
        notifier, to_rnti(pdu.pdu.rnti), pdu.pdu.slot, to_harq_id(pdu.harq_id), data, [this]() {
          state_machine.on_finish_processing_pdu();
        });

    trace_point tp = l1_ul_tracer.now();

    pusch_proc->process(data, std::move(rm_buffer2), processor_notifier, grid->get_reader(), pdu.pdu);

    l1_ul_tracer << trace_event("process_pusch", tp);
  });

  // Report the execution failure.
  if (!success) {
    logger.warning(pdu.pdu.slot.sfn(), pdu.pdu.slot.slot_index(), "Failed to execute PUSCH. Ignoring processing.");
    notify_discard_pusch(pdu);
  }
}

void uplink_processor_impl::process_pucch(const uplink_pdu_slot_repository::pucch_pdu& pdu)
{
  // Notify the creation of the execution task.
  if (!state_machine.on_create_pdu_task()) {
    return;
  }

  bool success = task_executors.pucch_executor.defer([this, &pdu]() {
    trace_point tp = l1_ul_tracer.now();

    pucch_processor_result proc_result;
    // Process the PUCCH.
    switch (pdu.context.format) {
      case pucch_format::FORMAT_0: {
        const auto& format0 = std::get<pucch_processor::format0_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid->get_reader(), format0);
        l1_ul_tracer << trace_event("pucch0", tp);
      } break;
      case pucch_format::FORMAT_1:
        // Do nothing.
        break;
      case pucch_format::FORMAT_2: {
        const auto& format2 = std::get<pucch_processor::format2_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid->get_reader(), format2);
        l1_ul_tracer << trace_event("pucch2", tp);
      } break;
      case pucch_format::FORMAT_3: {
        const auto& format3 = std::get<pucch_processor::format3_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid->get_reader(), format3);
        l1_ul_tracer << trace_event("pucch3", tp);
      } break;
      case pucch_format::FORMAT_4: {
        const auto& format4 = std::get<pucch_processor::format4_configuration>(pdu.config);
        proc_result         = pucch_proc->process(grid->get_reader(), format4);
        l1_ul_tracer << trace_event("pucch4", tp);
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
    state_machine.on_finish_processing_pdu();
  });

  // Report failed execution.
  if (!success) {
    logger.warning(
        pdu.context.slot.sfn(), pdu.context.slot.slot_index(), "Failed to execute PUCCH. Ignoring processing.");
    notify_discard_pucch(pdu);
  }
}

void uplink_processor_impl::process_pucch_f1(const uplink_pdu_slot_repository_impl::pucch_f1_collection& collection)
{
  // Notify the creation of the execution task.
  if (!state_machine.on_create_pdu_task()) {
    return;
  }

  bool success = task_executors.pucch_executor.defer([this, &collection]() {
    trace_point tp = l1_ul_tracer.now();

    // Process all PUCCH Format 1 in one go.
    const pucch_format1_map<pucch_processor_result>& results =
        pucch_proc->process(grid->get_reader(), collection.config);

    // Iterate each UE context.
    for (const auto& entry : collection.ue_contexts) {
      // Result for the given initial cyclic shift is not available.
      if (!results.contains(entry.initial_cyclic_shift, entry.time_domain_occ)) {
        logger.warning(collection.config.common_config.slot.sfn(),
                       collection.config.common_config.slot.slot_index(),
                       "Missing PUCCH F1 result for rnti={}, cs={} and occ={}.",
                       entry.context.rnti,
                       entry.initial_cyclic_shift,
                       entry.time_domain_occ);

        // Obtain number of HARQ-ACK.
        unsigned nof_harq_ack =
            collection.config.entries.get(entry.initial_cyclic_shift, entry.time_domain_occ).nof_harq_ack;

        // Report control-related discarded result if HARQ-ACK feedback is present.
        if (nof_harq_ack > 0) {
          ul_pucch_results discarded_results = ul_pucch_results::create_discarded(entry.context, nof_harq_ack);
          notifier.on_new_pucch_results(discarded_results);
        }
        continue;
      }

      // Write the results.
      ul_pucch_results notifier_result;
      notifier_result.context          = entry.context;
      notifier_result.processor_result = results.get(entry.initial_cyclic_shift, entry.time_domain_occ);

      // Notify the PUCCH results.
      notifier.on_new_pucch_results(notifier_result);
    }
    l1_ul_tracer << trace_event("pucch1", tp);
    state_machine.on_finish_processing_pdu();
  });

  // Notify the discarded transmissions if the executor failed.
  if (!success) {
    logger.warning(collection.config.common_config.slot.sfn(),
                   collection.config.common_config.slot.slot_index(),
                   "Failed to execute PUCCH. Ignoring processing.");
    notify_discard_pucch(collection);
  }
}

void uplink_processor_impl::process_srs(const uplink_pdu_slot_repository::srs_pdu& pdu)
{
  // Notify the creation of the execution task.
  if (!state_machine.on_create_pdu_task()) {
    return;
  }

  bool success = task_executors.srs_executor.defer([this, &pdu]() {
    trace_point tp = l1_ul_tracer.now();

    ul_srs_results result;
    result.context          = pdu.context;
    result.processor_result = srs->estimate(grid->get_reader(), pdu.config);

    l1_ul_tracer << trace_event("process_srs", tp);

    notifier.on_new_srs_results(result);
    state_machine.on_finish_processing_pdu();
  });

  if (!success) {
    logger.warning(
        pdu.context.slot.sfn(), pdu.context.slot.slot_index(), "Failed to execute SRS. Ignoring processing.");
    state_machine.on_finish_processing_pdu();
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

  state_machine.on_finish_processing_pdu();
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
  state_machine.on_finish_processing_pdu();
}

void uplink_processor_impl::notify_discard_pucch(const uplink_pdu_slot_repository_impl::pucch_f1_collection& collection)
{
  // Iterate all PUCCH Format 1 entries in the collection.
  for (const auto& entry : collection.ue_contexts) {
    // Obtain number of HARQ-ACK.
    unsigned nof_harq_ack =
        collection.config.entries.get(entry.initial_cyclic_shift, entry.time_domain_occ).nof_harq_ack;

    // Report control-related discarded result if HARQ-ACK feedback is present.
    if (nof_harq_ack > 0) {
      ul_pucch_results discarded_results = ul_pucch_results::create_discarded(entry.context, nof_harq_ack);
      notifier.on_new_pucch_results(discarded_results);
    }
  }
  state_machine.on_finish_processing_pdu();
}

void uplink_processor_impl::discard_slot()
{
  // Notify to the repository the discard of the slot. It skips the discard if the current state does not require it.
  if (!state_machine.start_discard_slot()) {
    return;
  }

  logger.warning(current_slot.sfn(), current_slot.slot_index(), "Discarded uplink slot. Ignoring processing.");

  // Iterate all symbols that have not been processed yet.
  for (unsigned i_symbol = nof_processed_symbols; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    for (const auto& pdu : pdu_repository.get_pucch_pdus(i_symbol)) {
      notify_discard_pucch(pdu);
    }

    for (const auto& collection : pdu_repository.get_pucch_f1_repository(i_symbol)) {
      notify_discard_pucch(collection);
    }

    for (const auto& pdu : pdu_repository.get_pusch_pdus(i_symbol)) {
      notify_discard_pusch(pdu);
    }

    for ([[maybe_unused]] const auto& pdu : pdu_repository.get_srs_pdus(i_symbol)) {
      state_machine.on_finish_processing_pdu();
    }
  }

  // Notify the end of discarding slot. The processor becomes idle.
  state_machine.finish_discard_slot();
}

std::atomic<unsigned>& uplink_processor_impl::get_grid_ref_counter()
{
  static std::vector<std::unique_ptr<std::atomic<unsigned>>> ref_counters;
  ref_counters.emplace_back(std::make_unique<std::atomic<unsigned>>(0));
  return *ref_counters.back();
}
