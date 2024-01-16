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

/// \brief Describes a low PAPR sequence collection as per TS38.211 Section 5.2.2.
///
/// This class describes a collection of \f$r^{(\alpha, \delta)}_{u,v}(n)\f$ sequences. The collection of sequences is
/// generated at construction time. The constructor is not suitable for real-time operation while the sequence retrieval
/// through method get() is suitable for real-time execution.
///
class low_papr_sequence_collection
{
public:
  /// Defines the number of sequence groups (\f$u\f$).
  static constexpr unsigned NOF_GROUPS = 30;

  /// Defines the number of sequence bases (\f$v\f$).
  static constexpr unsigned MAX_NOF_BASES = 2;

  /// Default destructor.
  virtual ~low_papr_sequence_collection() = default;

  /// \brief Get the sequence \f$r^{(\alpha, \delta)}_{u,v}(n)\f$ corresponding to the given indices.
  ///
  /// \param[in] u         Sequence group \f$u\f$ {0, ..., 29}.
  /// \param[in] v         Sequence base \f$v\f$ {0, 1}.
  /// \param[in] alpha_idx Corresponding index of parameter \f$\alpha\f$.
  /// \return A read-only view of the generated signal.
  /// \remark An assertion is triggered if any of \c u, \c v and \c alpha_idx is invalid.
  virtual span<const cf_t> get(unsigned u, unsigned v, unsigned alpha_idx) const = 0;
};

} // namespace srsran
