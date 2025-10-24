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

#include "srsran/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_generator.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// \brief Concurrent NZP-CSI-RS generator pool.
///
/// Wraps a concurrent pool of NZP-CSI-RS generators. It assigns a generator to each thread.
class nzp_csi_rs_generator_pool : public nzp_csi_rs_generator
{
public:
  /// NZP-CSI-RS generator pool type.
  using generator_pool_type = bounded_unique_object_pool<nzp_csi_rs_generator>;

  /// Creates a NZP-CSI-RS generator pool from a list of processors. Ownership is transferred to the pool.
  explicit nzp_csi_rs_generator_pool(std::shared_ptr<generator_pool_type> generators_) :
    logger(srslog::fetch_basic_logger("PHY")), generators(std::move(generators_))
  {
    srsran_assert(generators, "Invalid generator pool.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override
  {
    auto processor = generators->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve NZP-CSI-RS generator.");
      return;
    }
    processor->map(grid, config);
  }

private:
  srslog::basic_logger&                logger;
  std::shared_ptr<generator_pool_type> generators;
};

} // namespace srsran
