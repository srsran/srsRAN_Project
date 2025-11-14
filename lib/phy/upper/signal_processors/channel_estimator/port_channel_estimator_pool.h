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
#include "srsran/srsvec/zero.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// \brief Dummy port channel estimation reader.
///
/// Returned by the \c port_channel_estimator_pool when fetching a port channel estimator fails.
class port_channel_estimator_results_dummy : public port_channel_estimator_results
{
  // See interface for documentation.
  void get_symbol_ch_estimate(span<cbf16_t> symbol, unsigned i_symbol, unsigned tx_layer) const override
  {
    srsvec::zero(symbol);
  }

  // See interface for documentation.
  float get_epre() const override { return std::numeric_limits<float>::quiet_NaN(); }

  // See interface for documentation.
  float get_noise_variance() const override { return std::numeric_limits<float>::quiet_NaN(); }

  // See interface for documentation.
  float get_snr() const override { return std::numeric_limits<float>::quiet_NaN(); }

  // See interface for documentation.
  float get_rsrp(unsigned tx_layer) const override { return std::numeric_limits<float>::quiet_NaN(); }

  // See interface for documentation.
  std::optional<float> get_cfo_Hz() const override { return std::nullopt; }

  // See interface for documentation.
  phy_time_unit get_time_alignment() const override
  {
    return phy_time_unit::from_seconds(std::numeric_limits<float>::quiet_NaN());
  }
};

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
  const port_channel_estimator_results& compute(const resource_grid_reader& grid,
                                                unsigned                    port,
                                                const dmrs_symbol_list&     pilots,
                                                const configuration&        cfg) override
  {
    auto estimator = estimators->get();
    if (!estimator) {
      logger.error("Failed to retrieve a port channel estimator.");
      return dummy_results;
    }

    return estimator->compute(grid, port, pilots, cfg);
  }

private:
  srslog::basic_logger& logger;
  // Pool of port channel estimators.
  std::shared_ptr<estimator_pool> estimators;
  // Dummy reader for replacing missing estimators.
  port_channel_estimator_results_dummy dummy_results;
};

} // namespace srsran
