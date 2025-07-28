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

#include "srsran/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// \brief Concurrent SSB processor pool.
///
/// Wraps a concurrent pool of SSB processors. It assigns a SSB processor to each thread that processes SSB.
class ssb_processor_pool : public ssb_processor
{
public:
  /// SSB processor pool type.
  using ssb_processor_pool_type = bounded_unique_object_pool<ssb_processor>;

  /// Creates a SSB processor pool from a list of processors. Ownership is transferred to the pool.
  explicit ssb_processor_pool(std::shared_ptr<ssb_processor_pool_type> processors_) :
    logger(srslog::fetch_basic_logger("PHY")), processors(std::move(processors_))
  {
    srsran_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdu_t& pdu) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(pdu.slot.sfn(), pdu.slot.slot_index(), "Failed to retrieve SS/PBCH block processor.");
      return;
    }
    return processor->process(grid, pdu);
  }

private:
  srslog::basic_logger&                    logger;
  std::shared_ptr<ssb_processor_pool_type> processors;
};

} // namespace srsran
