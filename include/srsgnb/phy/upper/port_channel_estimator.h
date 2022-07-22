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
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// DM-RS-based channel estimator for one receive antenna port.
class port_channel_estimator
{
public:
  /// Estimator configuration parameters.
  struct configuration {
    /// Resource blocks for which the channel should be estimated (binary mask).
    bounded_bitset<MAX_RB> rb_mask = {};
    /// Subcarrier spacing of the estimated channel.
    subcarrier_spacing scs = subcarrier_spacing::kHz15;
    /// Cyclic prefix.
    cyclic_prefix c_prefix = cyclic_prefix::NORMAL;
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
  /// \param[in]  mask     Boolean mask with the positions of the DM-RS symbols.
  /// \param[in]  cfg      Estimator configuration.
  virtual void compute(channel_estimate&            estimate,
                       const resource_grid_reader&  grid,
                       unsigned                     port,
                       span<const dmrs_symbol_list> symbols,
                       span<const dmrs_mask>        mask,
                       const configuration&         cfg) = 0;
};

} // namespace srsgnb
