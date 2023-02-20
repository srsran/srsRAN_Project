/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/srsgnb_assert.h"
#include "srsgnb/support/units.h"

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
  srsgnb_assert(R > 0.F && R < 1.F, "Invalid target code rate {}, expected a value between 0 and 1", R);

  using namespace units::literals;
  if (A <= 292_bits || R <= 0.25F || (A <= 3824_bits && R <= 0.67F)) {
    return ldpc_base_graph_type::BG2;
  }

  return ldpc_base_graph_type::BG1;
}

} // namespace srsran
