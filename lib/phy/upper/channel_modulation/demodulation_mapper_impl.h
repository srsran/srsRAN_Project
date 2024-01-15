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
/// \brief Demodulator implementation.

#pragma once

#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"

namespace srsran {

/// \brief Approximate-LLR demodulator.
///
/// Implementation of the interface provided by demodulation_mapper. Instead of the defining expression, the
/// log-likelihood ratios are computed using the widely accepted approximation
/// \f[
/// \ell(b_{m+i}) = -\frac{1}{\sigma_m^2} \biggl( \min_{s\in \mathcal{S}_{i,0}} \abs{y_m - s}^2
///               - \min_{s\in \mathcal{S}_{i,1}} \abs{y_m - s}^2 \biggr)
/// \f]
/// where \f$y_m\f$ denotes the noisy modulation symbol, \f$\sigma_m^2\f$ is the equivalent (after equalization) noise
/// variance and \f$\mathcal{S}_{i,B}\f$ is the set of constellation points for which \f$b_{m+i}=B\in\{0,1\}\f$.
class demodulation_mapper_impl : public demodulation_mapper
{
public:
  // See interface for the documentation.
  void demodulate_soft(span<log_likelihood_ratio> llrs,
                       span<const cf_t>           symbols,
                       span<const float>          noise_vars,
                       modulation_scheme          mod) override;
};

} // namespace srsran
