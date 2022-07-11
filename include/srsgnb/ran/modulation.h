/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

namespace srsgnb {

enum qam_modulation : uint8_t { qam4 = 0, qam16, qam64, qam256 };

/// Calculates QAM modulation order, i.e. the number of bits used per symbol.
constexpr inline unsigned to_qam_modulation_order(qam_modulation q)
{
  return (2U * static_cast<unsigned>(q) + 2U);
}

/// Calculates number of combinations possible for a given qam modulation. e.g. 256-QAM -> 256.
constexpr inline unsigned to_nof_combinations(qam_modulation q)
{
  return 1U << to_qam_modulation_order(q);
}

} // namespace srsgnb
