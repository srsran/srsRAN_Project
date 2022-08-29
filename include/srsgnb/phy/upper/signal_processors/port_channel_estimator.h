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
/// \brief Channel estimator declaration.

#pragma once

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/re_measurement.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// Container for DM-RS symbols.
using dmrs_symbol_list = static_re_measurement<cf_t,
                                               pusch_constants::MAX_NOF_DMRS_SUBC,
                                               pusch_constants::MAX_NOF_DMRS_SYMBOLS,
                                               pusch_constants::MAX_NOF_LAYERS>;

/// DM-RS-based channel estimator for one receive antenna port.
class port_channel_estimator
{
public:
  /// Estimator configuration parameters.
  struct configuration {
    /// Resource blocks for which the channel should be estimated (binary mask).
    bounded_bitset<MAX_RB> rb_mask;
    /// Subcarrier spacing of the estimated channel.
    subcarrier_spacing scs = subcarrier_spacing::kHz15;
    /// First OFDM symbol within the slot for which the channel should be estimated.
    unsigned first_symbol = 0;
    /// Number of OFDM symbols for which the channel should be estimated.
    unsigned nof_symbols = 0;
    /// Number of transmit layers.
    unsigned nof_tx_layers = 0;
    /// List of receive ports.
    static_vector<uint8_t, DMRS_MAX_NPORTS> rx_ports;
  };

  /// Default destructor.
  virtual ~port_channel_estimator() = default;

  /// \brief Computes the channel estimate.
  ///
  /// Estimates the channel to the current receive antenna port for all transmit layers.
  /// \param[out] estimate The estimated channel coefficients for all resource elements, all OFDM symbols, all receive
  ///                      ports and all transmit layers.
  /// \param[in]  grid     The frequency&ndash;time resource grid.
  /// \param[in]  port     Receive antenna port the estimation refers to.
  /// \param[in]  symbols  DM-RS symbols (a.k.a. pilots). For each layer, symbols are listed by RE first and then by
  ///                      OFDM symbol.
  /// \param[in]  pattern  Pattern with the positions of the DM-RS symbols.
  /// \param[in]  cfg      Estimator configuration.
  virtual void compute(channel_estimate&           estimate,
                       const resource_grid_reader& grid,
                       unsigned                    port,
                       const dmrs_symbol_list&     symbols,
                       span<const dmrs_pattern>    pattern,
                       const configuration&        cfg) = 0;
};

} // namespace srsgnb
