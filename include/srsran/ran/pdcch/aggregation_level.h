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

#include "srsran/support/error_handling.h"
#include <cstdint>

namespace srsran {

/// Aggregation Level of PDCCH allocation.
enum class aggregation_level : uint8_t { n1 = 0, n2, n4, n8, n16 };

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
inline aggregation_level to_aggregation_level(uint8_t aggr_lvl)
{
  switch (aggr_lvl) {
    case 1:
      return aggregation_level::n1;
    case 2:
      return aggregation_level::n2;
    case 4:
      return aggregation_level::n4;
    case 8:
      return aggregation_level::n8;
    case 16:
      return aggregation_level::n16;
    default:
      report_fatal_error("Invalid aggregation level={}", aggr_lvl);
  }
}

} // namespace srsran
