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

#include <cstdint>
#include <type_traits>

namespace srsran {

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

} // namespace srsran