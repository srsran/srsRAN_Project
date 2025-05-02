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

#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// \brief PDSCH processor pool.
///
/// It contains a PDSCH processor for each DL thread. The processors are assigned to threads when they are first
/// requested. An assertion is thrown if the number of threads processing PDSCH is larger than the number of PDSCH
/// processors in the pool.
class pdsch_processor_pool : public pdsch_processor
{
public:
  /// PDSCH processor pool type.
  using pdsch_processor_pool_type = concurrent_thread_local_object_pool<pdsch_processor>;

  /// Creates a PDSCH processor pool from a list of processors. Ownership is transferred to the pool.
  explicit pdsch_processor_pool(std::shared_ptr<pdsch_processor_pool_type> processors_) :
    processors(std::move(processors_))
  {
    srsran_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override
  {
    pdsch_processor& processor = processors->get();
    processor.process(grid, notifier, std::move(data), pdu);
  }

private:
  std::shared_ptr<pdsch_processor_pool_type> processors;
};

} // namespace srsran
