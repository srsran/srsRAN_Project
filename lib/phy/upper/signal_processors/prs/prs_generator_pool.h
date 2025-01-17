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

#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// \brief Concurrent PRS generator pool.
///
/// Wraps a concurrent pool of PRS generators. It assigns a generator to each thread.
class prs_generator_pool : public prs_generator
{
public:
  /// PRS generator pool type.
  using generator_pool_type = concurrent_thread_local_object_pool<prs_generator>;

  /// Creates a SSB processor pool from a list of processors. Ownership is transferred to the pool.
  explicit prs_generator_pool(std::shared_ptr<generator_pool_type> processors_) : processors(std::move(processors_))
  {
    srsran_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override
  {
    prs_generator& processor = processors->get();
    return processor.generate(grid, config);
  }

private:
  std::shared_ptr<generator_pool_type> processors;
};

} // namespace srsran