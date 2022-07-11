/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_PDCCH_AGGREGATION_LEVEL_H
#define SRSGNB_RAN_PDCCH_AGGREGATION_LEVEL_H

#include <cstdint>

namespace srsgnb {

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

} // namespace srsgnb

#endif // SRSGNB_RAN_PDCCH_AGGREGATION_LEVEL_H
