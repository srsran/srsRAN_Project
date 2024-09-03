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
/// \brief Transform precoding valid sizes.
///
/// A number of physical blocs for transform precoding is valid if satisfies \f$M_{RB} = 2^{\alpha _2}\cdot 3^{\alpha
/// _3}\cdot 5^{\alpha _5}\f$ where \f$\alpha _2\f$, \f$\alpha _3\f$ and \f$\alpha _3\f$ are non-negative integers.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/ran/resource_block.h"
#include <optional>

namespace srsran {

/// Gets a boolean span where each position indicates if it is a valid number of RB for transform precoding.
inline span<const bool> get_transform_precoding_valid_nof_prb()
{
  static constexpr std::array<bool, MAX_RB> mask = {
      false, true,  true,  true,  true,  true,  true,  false, true,  true,  true,  false, true,  false, false, true,
      true,  false, true,  false, true,  false, false, false, true,  true,  false, true,  false, false, true,  false,
      true,  false, false, false, true,  false, false, false, true,  false, false, false, false, true,  false, false,
      true,  false, true,  false, false, false, true,  false, false, false, false, false, true,  false, false, false,
      true,  false, false, false, false, false, false, false, true,  false, false, true,  false, false, false, false,
      true,  true,  false, false, false, false, false, false, false, false, true,  false, false, false, false, false,
      true,  false, false, false, true,  false, false, false, false, false, false, false, true,  false, false, false,
      false, false, false, false, false, false, false, false, true,  false, false, false, false, true,  false, false,
      true,  false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false,
      true,  false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false,
      true,  false, true,  false, false, false, false, false, false, false, false, false, false, false, false, false,
      false, false, false, false, true,  false, false, false, false, false, false, false, false, false, false, false,
      true,  false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false,
      false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false,
      false, true,  false, false, false, false, false, false, false, false, false, false, false, false, false, false,
      true,  false, false, true,  false, false, false, false, false, false, true,  false, false, false, false, false,
      true,  false, false, false, false, false, false, false, false, false, false, false, false, false, true,  false,
      false, false, false};
  return mask;
}

/// Determines whether a number of PRB is valid.
inline bool is_transform_precoding_nof_prb_valid(unsigned nof_prb)
{
  span<const bool> valid_nof_prb = get_transform_precoding_valid_nof_prb();
  if (nof_prb >= valid_nof_prb.size()) {
    return false;
  }
  return valid_nof_prb[nof_prb];
}

/// \brief Gets the nearest valid of PRB for transform precoding.
/// \return A number of PRB equal to or higher than the given number of PRB.
inline std::optional<unsigned> get_transform_precoding_nearest_higher_nof_prb_valid(unsigned nof_prb)
{
  span<const bool> valid_nof_prb = get_transform_precoding_valid_nof_prb();
  if (nof_prb > valid_nof_prb.size()) {
    return std::nullopt;
  }

  auto nearest = std::find(valid_nof_prb.begin() + nof_prb, valid_nof_prb.end(), true);
  if (nearest == valid_nof_prb.end()) {
    return std::nullopt;
  }

  return std::distance(valid_nof_prb.begin(), nearest);
}

/// \brief Gets the nearest valid of PRB for transform precoding.
/// \return A number of PRB equal to or lower than the given number of PRB.
inline std::optional<unsigned> get_transform_precoding_nearest_lower_nof_prb_valid(unsigned nof_prb)
{
  span<const bool> valid_nof_prb = get_transform_precoding_valid_nof_prb();
  if (nof_prb > valid_nof_prb.size()) {
    return std::nullopt;
  }

  // Limit search to the first PRB.
  valid_nof_prb = valid_nof_prb.first(nof_prb);

  auto nearest = std::find(valid_nof_prb.rbegin(), valid_nof_prb.rend(), true);
  if (nearest == valid_nof_prb.rend()) {
    return std::nullopt;
  }

  return std::distance(valid_nof_prb.begin(), (++nearest).base());
}

} // namespace srsran
