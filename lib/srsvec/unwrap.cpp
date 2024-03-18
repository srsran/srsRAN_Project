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
/// \brief Phase angle unwrapping definitions.

#include "srsran/srsvec/unwrap.h"
#include <algorithm>
#include <cmath>

using namespace srsran;

/// \brif Unwraps a list of floats.
///
/// The inputs are assumed to take values in the range \f$[-\alpha, \alpha)\f$. Whenever the jump between two
/// consecutive values is larger than \f$\alpha\f$, the second value is shifted by \f$\pm 2 \alpha\f$, depending on the
/// sign of the jump. The typical application is the unwrapping of phase angles with \f$\alpha = \pi\f$.
///
/// \param[in,out] list Input and output list of numbers.
/// \param[in]     tol  The tolerance \f$\alpha\f$.
///
/// \warning The list of numbers is modified in-place.
static void unwrap_list(span<float> list, float tol = static_cast<float>(M_PI))
{
  float width = std::abs(tol);
  float k     = 0;

  auto* a_last = std::prev(list.end());
  for (auto* a = list.begin(); a != a_last; ++a) {
    float old_a  = *a;
    float next_a = *std::next(a);

    *a += 2.0F * k * width;

    float jump = next_a - old_a;
    if (std::abs(jump) > width) {
      k = k - std::copysign(1.0F, jump);
    }
  }

  *a_last += 2.0F * k * width;
}

void srsvec::unwrap_arguments(span<float> arguments, span<const cf_t> cplx_numbers)
{
  srsran_assert(arguments.size() == cplx_numbers.size(),
                "Input and output lists should have the same length, given {} and {}.",
                arguments.size(),
                cplx_numbers.size());

  std::transform(cplx_numbers.begin(), cplx_numbers.end(), arguments.begin(), [](cf_t a) { return std::arg(a); });

  unwrap_list(arguments);
}
