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
#include <type_traits>

namespace srsgnb {

/// \c ServCellIndex, as per TS 38.331. It concerns a short identity, used to uniquely identify a serving cell (from
/// a UE's perspective) across cell groups. Value 0 applies to the PCell (Master Cell Group).
enum serv_cell_index_t : uint8_t {
  SERVING_CELL_PCELL_IDX = 0,
  MAX_SERVING_CELL_IDX   = 31,
  MAX_NOF_SCELLS         = 31,
  MAX_NOF_SERVING_CELLS  = 32,
  SERVING_CELL_INVALID   = MAX_NOF_SERVING_CELLS
};

inline serv_cell_index_t to_serv_cell_index(std::underlying_type_t<serv_cell_index_t> val)
{
  return static_cast<serv_cell_index_t>(val);
}

} // namespace srsgnb