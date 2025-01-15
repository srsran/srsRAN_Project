/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/error_handling.h"
#include <cstdint>

namespace srsran {

/// Aggregation Level of PDCCH allocation.
enum class aggregation_level : uint8_t { n1 = 0, n2, n4, n8, n16 };

/// Number of possible aggregation levels.
const size_t NOF_AGGREGATION_LEVELS = 5;

/// Calculates number of CCEs based on Aggregation Level as per TS38.211 Table 7.3.2.1-1.
inline unsigned to_nof_cces(aggregation_level lvl)
{
  return 1U << static_cast<uint8_t>(lvl);
}

/// Calculates the index associated to the passed aggregation level.
inline unsigned to_aggregation_level_index(aggregation_level lvl)
{
  return static_cast<unsigned>(lvl);
}

/// Return aggregation level enum value.
inline aggregation_level aggregation_index_to_level(uint8_t aggr_lvl_idx)
{
  return static_cast<aggregation_level>(aggr_lvl_idx);
}

} // namespace srsran
