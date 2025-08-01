/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_processors/pdsch/formatters.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/srslog/logger.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"
#include <memory>
#include <vector>

namespace srsran {

/// PDSCH processor wrapper. It appends its identifier into the free list when the processing is finished.
class pdsch_processor_wrapper : public pdsch_processor, private pdsch_processor_notifier
{
public:
  /// Pool of PDSCH processor wrappers.
  using pool = bounded_unique_object_pool<pdsch_processor_wrapper>;

  /// Creates a PDSCH processor wrapper from another PDSCH processor.
  explicit pdsch_processor_wrapper(std::unique_ptr<pdsch_processor> processor_) : processor(std::move(processor_))
  {
    srsran_assert(processor, "Invalid PDSCH processor.");
  }

  /// Creates a PDSCH processor wrapper from another PDSCH processor wrapper.
  pdsch_processor_wrapper(pdsch_processor_wrapper&& other) noexcept :
    unique_token(std::move(other.unique_token)), notifier(other.notifier), processor(std::move(other.processor))
  {
    other.notifier = nullptr;
  }

  // See pdsch_processor interface for documentation.
  void process(resource_grid_writer&                                                            grid,
               pdsch_processor_notifier&                                                        notifier_,
               static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdsch_processor::pdu_t&                                                    pdu) override
  {
    // Save original notifier.
    notifier = &notifier_;

    // Process.
    processor->process(grid, *this, std::move(data), pdu);
  }

  /// Sets the unique pointer that will be released upon the completion of the PDSCH processing.
  void set_unique_ptr(pool::ptr unique_token_) { unique_token = std::move(unique_token_); }

private:
  // See pdsch_processor_notifier for documentation.
  void on_finish_processing() override
  {
    srsran_assert(notifier != nullptr, "Invalid notifier.");

    // Notify the completion of the processing.
    notifier->on_finish_processing();

    // Return the PDSCH processor identifier to the free list.
    unique_token = nullptr;
  }

  /// Processor identifier within the pool.
  pool::ptr unique_token;
  /// Current PDSCH processor notifier.
  pdsch_processor_notifier* notifier = nullptr;
  /// Wrapped PDSCH processor.
  std::unique_ptr<pdsch_processor> processor;
};

/// \brief Asynchronous PDSCH processor pool.
///
/// It contains PDSCH processors that are asynchronously executed. The processing of a PDSCH transmission is dropped if
/// there are no free PDSCH processors available.
class pdsch_processor_pool : public pdsch_processor
{
public:
  explicit pdsch_processor_pool(span<std::unique_ptr<pdsch_processor_wrapper>> processors_) :
    logger(srslog::fetch_basic_logger("PHY")), pool(processors_)
  {
  }

  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override
  {
    // Get a worker.
    auto unique_processor = pool.get();

    // If no worker is available.
    if (!unique_processor) {
      logger.warning(
          pdu.slot.sfn(), pdu.slot.slot_index(), "Insufficient number of PDSCH processors. Dropping PDSCH {:s}.", pdu);
      notifier.on_finish_processing();
      return;
    }

    // Save reference to the processor.
    pdsch_processor& processor = *unique_processor;

    // Set unique pointer
    unique_processor->set_unique_ptr(std::move(unique_processor));

    // Process PDSCH.
    processor.process(grid, notifier, std::move(data), pdu);
  }

private:
  srslog::basic_logger&         logger;
  pdsch_processor_wrapper::pool pool;
};

} // namespace srsran
