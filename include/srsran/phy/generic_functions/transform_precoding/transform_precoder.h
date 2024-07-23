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
/// \brief Transform precoding interfaces.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {

/// \brief Transform precoder interface.
///
/// The transform precoding process is described in TS38.211 Section 6.3.1.4 for PUSCH and Section 6.3.2.6.4 for PUCCH.
///
class transform_precoder
{
public:
  /// Default destructor.
  virtual ~transform_precoder() = default;

  /// \brief Reverts the transform precoding for a single OFDM symbol.
  ///
  /// The number of elements to deprecode \f$M_{sc}\f$ determines the number of resource blocks
  /// \f$M_{RB} = \frac{M_{sc}}{N_{sc}^{RB}}\f$.
  ///
  /// The function \ref is_transform_precoding_nof_prb_valid determines whether a number of resource blocks is valid.
  ///
  /// \param[out] out Transform deprecoding output.
  /// \param[in]  in  Transform deprecoding input.
  /// \remark An assertion is triggered if the size of \c in and \c out are not equal.
  /// \remark An assertion is triggered if the number of RB to deprecode is invalid.
  /// \remark An assertion is triggered if the number of RB exceeds the maximum initialized number of RB.
  virtual void deprecode_ofdm_symbol(span<cf_t> out, span<const cf_t> in) = 0;
};

} // namespace srsran
