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

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {

/// Describes a low PAPR sequence generator interface that implements TS38.211 Section 5.2.2 for any possible length.
class low_papr_sequence_generator
{
public:
  /// Default destructor.
  virtual ~low_papr_sequence_generator() = default;

  /// \brief Generates a \f$r^{(\alpha, \delta)}_{u,v}\f$ sequence as per TS38.211 Section 5.2.2.
  ///
  /// The cyclic shift \f$\alpha=2\pi\frac{n_{cs}}{n_{cs}^{max}}\f$ is split into the numerator and denominator.
  ///
  /// \param[out] sequence Provides the destination buffer and the sequence length.
  /// \param[in] u         Sequence group {0, ..., 29}.
  /// \param[in] v         Sequence number {0, 1}.
  /// \param[in] alpha_num Cyclic shift numerator, parameter \f$n_{cs}\f$.
  /// \param[in] alpha_den Cyclic shift denominator, parameter \f$n_{cs}^{max}\f$.
  virtual void generate(span<cf_t> sequence, unsigned u, unsigned v, unsigned alpha_num, unsigned alpha_den) = 0;
};

} // namespace srsran
