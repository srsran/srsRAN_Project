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

#include "srsran/phy/upper/channel_processors/pucch_processor.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// Concurrent PUCCH processor pool.
class pucch_processor_pool : public pucch_processor
{
public:
  /// Creates a PUCCH processor pool from a list of processors. Ownership is transferred to the pool.
  explicit pucch_processor_pool(std::vector<std::unique_ptr<pucch_processor>> processors_) :
    processors(std::move(processors_))
  {
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    pucch_processor& processor = processors.get();
    return processor.process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format1_configuration& config) override
  {
    pucch_processor& processor = processors.get();
    return processor.process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    pucch_processor& processor = processors.get();
    return processor.process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    pucch_processor& processor = processors.get();
    return processor.process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    pucch_processor& processor = processors.get();
    return processor.process(grid, config);
  }

private:
  concurrent_thread_local_object_pool<pucch_processor> processors;
};

} // namespace srsran