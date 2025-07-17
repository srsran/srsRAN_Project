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

#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"
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
