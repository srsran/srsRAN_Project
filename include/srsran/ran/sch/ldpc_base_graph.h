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

#include "srsran/support/srsran_assert.h"
#include "srsran/support/units.h"

namespace srsran {

/// LDPC Base graph types.
enum class ldpc_base_graph_type : uint8_t { BG1 = 1, BG2 = 2 };

/// \brief Returns the LDPC base graph based on the given code rate and transport block size in bits, as per TS 38.212
/// section 7.2.2.
///
/// \param R Target code rate, parameter \f$R\f$.
/// \param A Size in bits of the payload \f$A\f$.
constexpr ldpc_base_graph_type get_ldpc_base_graph(float R, units::bits A)
{
  srsran_assert(R > 0.F && R < 1.F, "Invalid target code rate {}, expected a value between 0 and 1", R);

  using namespace units::literals;
  if (A <= 292_bits || R <= 0.25F || (A <= 3824_bits && R <= 0.67F)) {
    return ldpc_base_graph_type::BG2;
  }

  return ldpc_base_graph_type::BG1;
}

} // namespace srsran
