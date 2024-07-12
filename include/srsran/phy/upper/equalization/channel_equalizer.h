/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief Channel equalizer interface.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/support/re_buffer.h"

namespace srsran {

/// \brief Channel equalizer interface.
///
/// The channel equalizer mitigates the radio propagation channel effects. Also, it reverts the layer mapping described
/// by TS38.211 Section 6.3.1.3.
class channel_equalizer
{
public:
  /// \brief Interface for the list of channel estimates.
  class ch_est_list
  {
  public:
    /// Default destructor.
    virtual ~ch_est_list() = default;

    /// Gets a read-only channel given a receive port and layer indices.
    virtual span<const cbf16_t> get_channel(unsigned i_rx_port, unsigned i_layer) const = 0;

    /// Gets the number of resource elements.
    virtual unsigned get_nof_re() const = 0;

    /// Gets the number of receive ports.
    virtual unsigned get_nof_rx_ports() const = 0;

    /// Gets the number of transmit layers.
    virtual unsigned get_nof_tx_layers() const = 0;
  };

  /// Default destructor.
  virtual ~channel_equalizer() = default;

  /// \brief Equalizes the MIMO channel and combines Tx&ndash;Rx paths.
  ///
  /// For each transmit layer, the contributions of all receive ports are combined with weights obtained from the
  /// estimated channel coefficients. The variance of the point-to-point equivalent noise perturbing each modulated
  /// symbol is also estimated.
  ///
  /// Also, it reverts the layer mapping described by TS38.211 Section 6.3.1.3.
  ///
  /// \param[out] eq_symbols          Equalized modulation symbols.
  /// \param[out] eq_noise_vars       Post-equalization noise variances.
  /// \param[in]  ch_symbols          Channel symbols, i.e., complex samples organized by receive port.
  /// \param[in]  ch_estimates        Channel estimation coefficients, indexed by receive port and transmission layer.
  /// \param[in]  noise_var_estimates Noise variance estimation for each receive port.
  /// \param[in]  tx_scaling          Transmission gain scaling factor.
  /// \note The sizes of \c eq_symbols, \c eq_noise_vars, \c ch_symbols and \c noise_var_estimates must be consistent
  /// with the \c ch_estimates channel dimensions.
  /// \warning If, for a given transmitted RE, the combined channel across all paths results in a non-normal value
  /// (zero, infinity or NaN), the corresponding equalized modulation RE and equalized noise variance will be set to
  /// zero and infinity, respectively.
  /// \warning If the \c noise_var_estimates noise variances have ill-formed values (zero, negative, infinity or NaN),
  /// the corresponding equalized modulation REs and equalized noise variances will be set to zero and infinity,
  /// respectively.
  virtual void equalize(span<cf_t>                       eq_symbols,
                        span<float>                      eq_noise_vars,
                        const re_buffer_reader<cbf16_t>& ch_symbols,
                        const ch_est_list&               ch_estimates,
                        span<const float>                noise_var_estimates,
                        float                            tx_scaling) = 0;
};

} // namespace srsran
