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

/// Channel equalizer interface.
class channel_equalizer
{
private:
  /// Dimension, i.e. number of coordinates, of each indexing level of the channel estimates.
  enum class ch_dims : unsigned { re = 0, rx_port = 1, tx_layer = 2, nof_dims = 3 };
  /// Dimension, i.e. number of coordinates, of each indexing level of the Resource Elements.
  enum class re_dims : unsigned { re = 0, slice = 1, nof_dims = 2 };

public:
  /// \brief Container for input and output Resource Elements.
  /// \remark Dimension indexing given by \ref channel_equalizer::re_dims
  using re_list = tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), cf_t, re_dims>;

  /// Container for the post-equalization noise variances.
  /// \remark Dimension indexing given by \ref channel_equalizer::re_dims
  using noise_var_list = tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), float, re_dims>;

  /// Container for the channel estimates.
  /// \remark Dimension indexing given by \ref channel_equalizer::ch_dims
  using ch_est_list = tensor<std::underlying_type_t<ch_dims>(ch_dims::nof_dims), cf_t, ch_dims>;

  /// Default destructor.
  virtual ~channel_equalizer() = default;

  /// \brief Equalizes the MIMO channel and combines Tx&ndash;Rx paths.
  ///
  /// For each transmit layer, the contributions of all receive ports are combined with weights obtained from the
  /// estimated channel coefficients. The variance of the point-to-point equivalent noise perturbing each modulated
  /// symbol is also estimated.
  ///
  /// \param[out] eq_symbols  Equalized modulation symbols, organized by transmission layer.
  /// \param[out] eq_noise_vars   Post-equalization noise variances, organized by transmission layer.
  /// \param[in]  ch_symbols   Channel symbols, i.e., complex samples organized by receive port.
  /// \param[in]  ch_estimates Channel estimation coefficients, indexed by receive port and transmission layer.
  /// \param[in]  noise_var_estimates Noise variance estimation for each receive port.
  /// \param[in]  tx_scaling   Transmission gain scaling factor.
  /// \note The sizes of \c eq_symbols, \c eq_noise_vars and \c ch_symbols must be consistent with the
  /// \c ch_estimates channel dimensions.
  virtual void equalize(re_list&           eq_symbols,
                        noise_var_list&    eq_noise_vars,
                        const re_list&     ch_symbols,
                        const ch_est_list& ch_estimates,
                        span<const float>  noise_var_estimates,
                        float              tx_scaling) = 0;
};

} // namespace srsgnb
