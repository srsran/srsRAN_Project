/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// \brief Concurrent NZP-CSI-RS generator pool.
///
/// Wraps a concurrent pool of NZP-CSI-RS generators. It assigns a generator to each thread.
class nzp_csi_rs_generator_pool : public nzp_csi_rs_generator
{
public:
  /// NZP-CSI-RS generator pool type.
  using generator_pool_type = concurrent_thread_local_object_pool<nzp_csi_rs_generator>;

  /// Creates a SSB processor pool from a list of processors. Ownership is transferred to the pool.
  explicit nzp_csi_rs_generator_pool(std::shared_ptr<generator_pool_type> processors_) :
    processors(std::move(processors_))
  {
    srsran_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override
  {
    nzp_csi_rs_generator& processor = processors->get();
    return processor.map(grid, config);
  }

private:
  std::shared_ptr<generator_pool_type> processors;
};

} // namespace srsran