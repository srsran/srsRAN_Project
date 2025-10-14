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

#include "srsran/phy/upper/signal_processors/channel_estimator/port_channel_estimator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// Concurrent port channel estimator.
class port_channel_estimator_pool : public port_channel_estimator
{
public:
  using estimator_pool = bounded_unique_object_pool<port_channel_estimator>;

  /// Creates a concurrent channel estimator from a shared pool of estimators.
  explicit port_channel_estimator_pool(std::shared_ptr<estimator_pool> estimators_) :
    logger(srslog::fetch_basic_logger("PHY")), estimators(std::move(estimators_))
  {
    srsran_assert(estimators, "Invalid port channel estimator pool.");
  }

  // See interface for documentation.
  void compute(channel_estimate&           estimate,
               const resource_grid_reader& grid,
               unsigned                    port,
               const dmrs_symbol_list&     pilots,
               const configuration&        cfg) override
  {
    auto estimator = estimators->get();
    if (!estimator) {
      logger.error("Failed to retrieve a port channel estimator.");
      return;
    }

    estimator->compute(estimate, grid, port, pilots, cfg);
  }

private:
  srslog::basic_logger& logger;
  // Pool of port channel estimators.
  std::shared_ptr<estimator_pool> estimators;
};

} // namespace srsran
