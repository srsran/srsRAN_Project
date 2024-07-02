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

#pragma once

#include "srsran/adt/concurrent_queue.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
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
  void process(resource_grid_mapper&                                        mapper,
               pdsch_processor_notifier&                                    notifier,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override
  {
    pdsch_processor& processor = processors->get();
    return processor.process(mapper, notifier, data, pdu);
  }

private:
  std::shared_ptr<pdsch_processor_pool_type> processors;
};

} // namespace srsran
