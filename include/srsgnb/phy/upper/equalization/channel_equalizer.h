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

#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/phy/upper/re_measurement.h"

namespace srsgnb {

/// Container for equalized symbols.
using equalizer_symbol_list = re_measurement<cf_t>;

/// Container for the noise variance.
using equalizer_noise_var_list = re_measurement<float>;

/// Container for the channel symbols.
using equalizer_ch_symbol_list = re_measurement<cf_t>;

/// Channel equalizer interface.
class channel_equalizer
{
public:
  /// Default destructor.
  virtual ~channel_equalizer() = default;

  /// \brief Equalizes the MIMO channel and combines Tx&ndash;Rx paths.
  ///
  /// For each transmit layer, the contributions of all receive ports are combined with weights obtained from the
  /// estimated channel coefficients. The variance of the point-to-point equivalent noise perturbing each modulated
  /// symbol is also estimated.
  ///
  /// \param[out] mod_symbols  Estimated modulation symbols.
  /// \param[out] noise_vars   Estimated noise variances (after equalization).
  /// \param[in]  ch_symbols   Channel symbols, i.e., complex samples from the receive ports.
  /// \param[in]  ch_estimates Channel coefficients (typically provided by the channel estimator).
  /// \param[in]  tx_scaling   Transmission gain scaling factor.
  /// \note The sizes of \c mod_symbols, \c noise_vars and \c ch_symbols must be consistent with the
  /// \c ch_estimates channel dimensions.
  virtual void equalize(equalizer_symbol_list&          mod_symbols,
                        equalizer_noise_var_list&       noise_vars,
                        const equalizer_ch_symbol_list& ch_symbols,
                        const channel_estimate&         ch_estimates,
                        float                           tx_scaling) = 0;
};

} // namespace srsgnb
