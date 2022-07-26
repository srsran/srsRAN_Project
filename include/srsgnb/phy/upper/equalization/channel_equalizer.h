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
/// \brief Channel equalizer interface.

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"

namespace srsgnb {

/// Channel equalizer interface.
class channel_equalizer
{
public:
  /// Equalizer configuration parameters.
  struct configuration {
    /// Resource blocks for which the channel should be estimated (binary mask).
    bounded_bitset<MAX_RB> rb_mask = {};
    /// First OFDM symbol within the slot for which the channel should be estimated.
    unsigned first_symbol = 0;
    /// Number of OFDM symbols for which the channel should be estimated.
    unsigned nof_symbols = 0;
    /// Number of transmit layers.
    unsigned nof_tx_layers = 0;
    /// Number of receive ports.
    unsigned nof_rx_ports = 0;
  };

  /// Default destructor.
  virtual ~channel_equalizer() = default;

  /// \brief Equalizes the MIMO channel and combines Tx&ndash;Rx paths.
  ///
  /// For each transmit layer, the contributions of all receive ports are combined with weights obtain from the
  /// estimated channel coefficients. The method also takes care of extracting data-carrying symbols, thus reverting the
  /// layer mapping operation described in TS38.211 Section 6.3.1.3. The variance of the point-to-point equivalent noise
  /// perturbing each modulated symbol is also estimated.
  ///
  /// \param[out] mod_symbols Estimated modulation symbols.
  /// \param[out] noise_vars  Estimated noise variances (after equalization).
  /// \param[in]  grid        The frequency&ndash;time resource grid for all paths.
  /// \param[in]  ch_estimate Channel coefficients (typically provided by the channel estimator).
  /// \parm[in]   config      Configuration parameters.
  virtual void equalize(span<cf_t>                  mod_symbols,
                        span<float>                 noise_vars,
                        const resource_grid_reader& grid,
                        const channel_estimate&     ch_estimate,
                        const configuration&        config) = 0;
};

} // namespace srsgnb