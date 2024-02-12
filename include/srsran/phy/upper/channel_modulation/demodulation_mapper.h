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
/// \brief Demodulator interface.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/sch/modulation_scheme.h"

namespace srsran {

/// \brief Demodulator interface.
///
/// The demodulation mapper reverts the operations described in TS38.211 Section 5.1. It takes as input a sequence of
/// noisy, complex-valued modulation symbols and it returns a sequence of log-likelihood ratios (soft bits). More
/// specifically, the \f$m\f$th modulation symbol will be mapped to the soft bits
/// \f[
/// \ell(b_{m+i}) = \log \frac{\Pr(b_{m+i}=0|y_m)}{\Pr(b_{m+i}=1|y_m)}
/// \f]
/// where \f$i = 0,1,\dots,Q_{\textup{m}}-1\f$, with \f$Q_{\textup{m}}\f$ denoting the modulation order as defined in
/// TS38.211 Table 6.3.1.2-1, and where \f$\Pr(b_{m+i}=B|y_m)\f$ is the conditional probability of bit \f$b_{m+i}\f$
/// being equal to \f$B\f$ given the modulation symbol \f$y_m\f$.
class demodulation_mapper
{
public:
  /// Default destructor.
  virtual ~demodulation_mapper() = default;

  /// \brief Soft demodulation.
  ///
  /// Demodulates complex symbols into log-likelihood ratios (i.e., soft bits).
  /// \param[out] llrs       Demodulated log-likelihood ratios.
  /// \param[in]  symbols    Complex symbols to demodulate (equalization output).
  /// \param[in]  noise_vars Noise variances (after equalization).
  /// \param[in]  mod        Modulation scheme.
  /// \remark Parameters \c symbols and \c noise_vars must have the same length. Each entry of \c noise_vars
  ///         contains the (estimated) noise variance after equalization corresponding to the symbol with the same
  ///         index.
  /// \remark The length of \c llrs is given by the number of symbols multiplied by the modulation order of \c mod.
  /// \remark Any operation involving ill-formed inputs (NaN, infinity or negative noise variances, as well as
  /// NaN-valued symbols or infinity-over-infinity divisions) will result in an indeterminate, zero-valued log-
  /// likelihood ratio.
  virtual void demodulate_soft(span<log_likelihood_ratio> llrs,
                               span<const cf_t>           symbols,
                               span<const float>          noise_vars,
                               modulation_scheme          mod) = 0;
};

} // namespace srsran
