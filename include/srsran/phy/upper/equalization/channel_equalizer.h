/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Channel equalizer interface.

#pragma once

#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/re_measurement.h"

namespace srsran {

/// \brief Channel equalizer interface.
///
/// The channel equalizer mitigates the radio propagation channel effects. Also, it reverts the layer mapping described
/// by TS38.211 Section 6.3.1.3.
class channel_equalizer
{
private:
  /// Dimensions, i.e., number of coordinates, spanned by each indexing level of the Resource Element data.
  enum class re_dims : unsigned {
    /// Resource Element.
    re = 0,
    /// Set of all REs corresponding to a single receive port or a single transmit layer.
    slice = 1,
    /// Total number of dimensions.
    nof_dims = 2
  };

  /// Dimensions, i.e. number of coordinates, spanned by each indexing level of the channel estimation data.
  enum class ch_dims : unsigned {
    /// Channel coefficient for a single Resource Element and a single Tx&ndash;Rx channel path.
    re = 0,
    /// Set of all channel coefficients corresponding to a single Tx&ndash;Rx channel path.
    rx_port = 1,
    /// Set of all channel coefficients corresponding to all Tx&ndash;Rx paths for a single Tx layer.
    tx_layer = 2,
    /// Total number of dimensions.
    nof_dims = 3
  };

public:
  /// \brief Container for input and output Resource Elements.
  /// \remark Dimension indexing given by \ref channel_equalizer::re_dims.
  using re_list = tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), cf_t, re_dims>;

  /// \brief Container for the channel estimates.
  /// \remark Dimension indexing given by \ref channel_equalizer::ch_dims.
  using ch_est_list = tensor<std::underlying_type_t<ch_dims>(ch_dims::nof_dims), cf_t, ch_dims>;

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
  virtual void equalize(span<cf_t>         eq_symbols,
                        span<float>        eq_noise_vars,
                        const re_list&     ch_symbols,
                        const ch_est_list& ch_estimates,
                        span<const float>  noise_var_estimates,
                        float              tx_scaling) = 0;
};

} // namespace srsran
