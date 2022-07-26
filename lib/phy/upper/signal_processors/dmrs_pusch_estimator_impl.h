/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH channel estimator definition.

#pragma once

#include "srsgnb/phy/upper/port_channel_estimator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"

namespace srsgnb {

/// PUSCH DM-RS generator.
class dmrs_pusch_generator_impl
{
public:
  // Aliases
  /// Gathers all PUSCH DM-RS configuration parameters.
  using configuration = dmrs_pusch_estimator::configuration;

  /// \brief Generates the PUSCH DM-RS symbols for one transmission port.
  ///
  /// Implements the PUSCH DM-RS generation, precoding and mapping procedures described in TS38.211 Section 6.4.1.1.
  /// \param[out] symbols Lists of generated DM-RS symbols, one per transmission layer.
  /// \param[out] mask    Lists of boolean maps representing the REs the DM-RS symbols should be mapped to, one per
  ///                     transmission layer.
  /// \param[in]  cfg     Configuration parameters.
  void generate(span<dmrs_symbol_list> symbols, span<dmrs_mask> mask, const configuration& cfg);

  // todo(david): finish this
};

class dmrs_pusch_estimator_impl : public dmrs_pusch_estimator
{
public:
  /// Constructor - sets the channel estimator.
  explicit dmrs_pusch_estimator_impl(std::unique_ptr<port_channel_estimator> ch_est) : ch_estimator(std::move(ch_est))
  {
  }

  // See interface for the documentation.
  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) override;

private:
  /// Maximum supported number of transmission layers.
  static constexpr unsigned MAX_TX_LAYERS = pusch_constants::MAX_NOF_LAYERS;

  /// Antenna port channel estimator.
  std::unique_ptr<port_channel_estimator> ch_estimator;
  /// DM-RS generator.
  dmrs_pusch_generator_impl dmrs_gen = {};
  /// Buffer for DM-RS symbols.
  static_vector<dmrs_symbol_list, MAX_TX_LAYERS> symbols;
  /// Buffer for DM-RS symbol coordinates.
  static_vector<dmrs_mask, MAX_TX_LAYERS> coordinates;
};

} // namespace srsgnb
