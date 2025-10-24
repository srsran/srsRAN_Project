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

#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// Concurrent PUCCH processor pool.
class pucch_processor_pool : public pucch_processor
{
public:
  using processor_pool = bounded_unique_object_pool<pucch_processor>;

  /// Creates a PUCCH processor pool from a shared pool of processors.
  explicit pucch_processor_pool(std::shared_ptr<processor_pool> processors_) :
    logger(srslog::fetch_basic_logger("PHY")), processors(std::move(processors_))
  {
    srsran_assert(processors, "Invalid PUCCH processor pool.");
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 0.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.common_config.slot.sfn(),
                   config.common_config.slot.slot_index(),
                   "Failed to retrieve PUCCH processor for Format 1.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 2.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 3.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 4.");
      return {};
    }
    return processor->process(grid, config);
  }

private:
  srslog::basic_logger&           logger;
  std::shared_ptr<processor_pool> processors;
};

} // namespace srsran
