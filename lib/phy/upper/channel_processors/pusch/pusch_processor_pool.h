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

#pragma once

#include "srsran/adt/mpmc_queue.h"
#include "srsran/phy/upper/channel_processors/pusch/formatters.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// PUSCH processor wrapper. It appends its identifier into the free list when the processing is finished.
class pusch_processor_wrapper : public pusch_processor, private pusch_processor_result_notifier
{
public:
  /// Pool of PUSCH processor wrappers.
  using pool = bounded_unique_object_pool<pusch_processor_wrapper>;

  /// Creates a pusch processor wrapper from another pusch processor.
  explicit pusch_processor_wrapper(std::unique_ptr<pusch_processor> processor_) : processor(std::move(processor_))
  {
    srsran_assert(processor, "Invalid pusch processor.");
  }

  /// Creates a PUSCH processor wrapper from another PUSCH processor wrapper.
  pusch_processor_wrapper(pusch_processor_wrapper&& other) :
    unique_pool_ptr(std::move(other.unique_pool_ptr)), notifier(other.notifier), processor(std::move(other.processor))
  {
    other.notifier = nullptr;
  }

  // See pusch_processor interface for documentation.
  void process(span<uint8_t>                    data,
               unique_rx_buffer                 rm_buffer,
               pusch_processor_result_notifier& notifier_,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override
  {
    // Save original notifier.
    [[maybe_unused]] pusch_processor_result_notifier* prev_notifier = std::exchange(notifier, &notifier_);
    srsran_assert(prev_notifier == nullptr, "PUSCH processor is in use.");

    // Process.
    processor->process(data, std::move(rm_buffer), *this, grid, pdu);
  }

  /// Sets the unique pointer that will be released upon the completion of the PUSCH processing.
  void set_unique_ptr(pool::ptr unique_token_) { unique_pool_ptr = std::move(unique_token_); }

private:
  // See pusch_processor_result_notifier for documentation.
  void on_uci(const pusch_processor_result_control& uci) override
  {
    srsran_assert(notifier != nullptr, "Invalid notifier.");
    notifier->on_uci(uci);
  }

  // See pusch_processor_result_notifier for documentation.
  void on_sch(const pusch_processor_result_data& sch) override
  {
    // Notify the completion of the processing.
    pusch_processor_result_notifier* current_notifier = std::exchange(notifier, nullptr);
    srsran_assert(current_notifier != nullptr, "Invalid notifier.");
    current_notifier->on_sch(sch);

    // Return the pusch processor identifier to the free list.
    unique_pool_ptr = nullptr;
  }

  /// Processor identifier within the pool.
  pool::ptr unique_pool_ptr;
  /// Current PUSCH processor notifier.
  pusch_processor_result_notifier* notifier = nullptr;
  /// Wrapped PUSCH processor.
  std::unique_ptr<pusch_processor> processor;
};

/// \brief PUSCH processor pool
///
/// It contains PUSCH processors that are asynchronously executed. The processing of a PUSCH transmission is dropped if
/// there are no free PUSCH processors available.
///
class pusch_processor_pool : public pusch_processor
{
public:
  /// Alias for the common UCI processor pool. The processors of this pool operate synchronously from the thread that
  /// calls processing.
  using uci_processor_pool = bounded_unique_object_pool<pusch_processor>;

  /// Creates a PUSCH processor pool from a list of processors. Ownership is transferred to the pool.
  pusch_processor_pool(span<std::unique_ptr<pusch_processor_wrapper>> processors_,
                       std::shared_ptr<uci_processor_pool>            uci_processors_) :
    logger(srslog::fetch_basic_logger("PHY")), processors(processors_), uci_processors(uci_processors_)
  {
  }

  // See interface for documentation.
  void process(span<uint8_t>                    data,
               unique_rx_buffer                 rm_buffer,
               pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override
  {
    // Get a processor and process normally.
    auto unique_processor = processors.get();
    if (unique_processor) {
      // Save reference to the processor.
      pusch_processor& processor = *unique_processor;

      // Set unique pointer, it will be returned to the pool when the processing is completed.
      unique_processor->set_unique_ptr(std::move(unique_processor));

      // Process PUSCH.
      processor.process(data, std::move(rm_buffer), notifier, grid, pdu);
      return;
    }

    // No PUSCH processor is available and no UCI is present. Drop PUSCH reception.
    if ((pdu.uci.nof_harq_ack == 0) && (pdu.uci.nof_csi_part1 == 0)) {
      logger.warning(
          pdu.slot.sfn(), pdu.slot.slot_index(), "PUSCH processing queue is full. Dropping PUSCH {:s}.", pdu);

      // Report data-related discarded result if shared channel data is present.
      if (pdu.codeword.has_value()) {
        notifier.on_sch({.data = {.tb_crc_ok = false, .nof_codeblocks_total = 0, .ldpc_decoder_stats = {}}, .csi = {}});
      }

      return;
    }

    // Process UCI synchronously if UCI is present.
    uci_processor_pool::ptr uci_processor = uci_processors->get();
    if (uci_processor) {
      logger.warning(
          pdu.slot.sfn(), pdu.slot.slot_index(), "PUSCH processing queue is full. Processing UCI {:s}.", pdu);
      uci_processor->process(data, std::move(rm_buffer), notifier, grid, pdu);
    } else {
      logger.warning(
          pdu.slot.sfn(), pdu.slot.slot_index(), "PUSCH processing queue is full. Failed processing UCI {:s}.", pdu);

      // Report data-related discarded result if shared channel data is present.
      if (pdu.codeword.has_value()) {
        notifier.on_sch({.data = {.tb_crc_ok = false, .nof_codeblocks_total = 0, .ldpc_decoder_stats = {}}, .csi = {}});
      }

      pusch_processor_result_data discarded_results;
      notifier.on_uci({.harq_ack  = {.payload = uci_payload_type(pdu.uci.nof_harq_ack), .status = uci_status::invalid},
                       .csi_part1 = {},
                       .csi_part2 = {},
                       .csi       = {}});
    }
  }

private:
  /// Physical layer logger.
  srslog::basic_logger& logger;
  /// Actual PUSCH processor pool.
  pusch_processor_wrapper::pool processors;
  /// UCI processor pool, used only the normal processor pool runs out of processors or the PUSCH transmission only
  /// contains UCI.
  std::shared_ptr<uci_processor_pool> uci_processors;
};

} // namespace srsran
