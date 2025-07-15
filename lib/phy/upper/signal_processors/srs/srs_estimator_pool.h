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
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// Concurrent Sounding Reference Signal propagation channel estimator pool.
class srs_estimator_pool : public srs_estimator
{
public:
  using estimator_pool = bounded_object_pool<srs_estimator>;

  /// Creates a pool from a list of estimators. Ownership is transferred to the pool.
  explicit srs_estimator_pool(std::shared_ptr<estimator_pool> estimators_) :
    logger(srslog::fetch_basic_logger("PHY")), estimators(std::move(estimators_))
  {
  }

  // See interface for documentation.
  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    auto estimator = estimators->get();
    if (!estimator) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve NZP-CSI-RS generator.");
      return {};
    }
    return estimator->estimate(grid, config);
  }

private:
  srslog::basic_logger&           logger;
  std::shared_ptr<estimator_pool> estimators;
};

} // namespace srsran
