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

#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// Concurrent Sounding Reference Signal propagation channel estimator pool.
class srs_estimator_pool : public srs_estimator
{
public:
  /// Creates a pool from a list of estimators. Ownership is transferred to the pool.
  explicit srs_estimator_pool(std::vector<std::unique_ptr<srs_estimator>> estimators_) :
    estimators(std::move(estimators_))
  {
  }

  // See interface for documentation.
  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    return estimators.get().estimate(grid, config);
  }

private:
  concurrent_thread_local_object_pool<srs_estimator> estimators;
};

} // namespace srsran
