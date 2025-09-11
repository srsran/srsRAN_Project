/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
/// \brief Cumulative sum of sequences.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include <numeric>

namespace srsran {
namespace srsvec {

///@{
/// \brief Cumulative sum of a sequence.
/// \param[in] x  Input sequence.
/// \return The sum of all elements of the input sequence.
float accumulate(span<const float> x);

inline cf_t accumulate(span<const cf_t> x)
{
  return std::accumulate(x.begin(), x.end(), cf_t());
}
///@}

} // namespace srsvec
} // namespace srsran
