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

#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// \brief Concurrent PRS generator pool.
///
/// Wraps a concurrent pool of PRS generators. It assigns a generator to each thread.
class prs_generator_pool : public prs_generator
{
public:
  /// PRS generator pool type.
  using generator_pool_type = bounded_unique_object_pool<prs_generator>;

  /// Creates a SSB processor pool from a list of processors. Ownership is transferred to the pool.
  explicit prs_generator_pool(std::shared_ptr<generator_pool_type> processors_) :
    logger(srslog::fetch_basic_logger("PHY")), processors(std::move(processors_))
  {
    srsran_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error("Failed to retrieve PRS generator.");
      return;
    }
    return processor->generate(grid, config);
  }

private:
  srslog::basic_logger&                logger;
  std::shared_ptr<generator_pool_type> processors;
};

} // namespace srsran
