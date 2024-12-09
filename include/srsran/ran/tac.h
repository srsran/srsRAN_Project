/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

namespace srsran {

/// 3GPP TS 38.331, TrackingAreaCode ::= BIT STRING (SIZE (24)).
using tac_t                 = uint32_t;
constexpr tac_t INVALID_TAC = 16777216;

/// Returns true if the given struct is valid, otherwise false.
constexpr bool is_valid(tac_t tac)
{
  // TAC must be in [0..16777215].
  return tac < INVALID_TAC;
}

} // namespace srsran
