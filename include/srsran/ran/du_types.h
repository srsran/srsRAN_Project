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

#include "serv_cell_index.h"
#include <cstdint>
#include <type_traits>

namespace srsran {

/// Maximum number of UEs supported by DU (implementation-defined).
enum du_ue_index_t : uint16_t {
  MIN_DU_UE_INDEX     = 0,
  MAX_DU_UE_INDEX     = 1023,
  MAX_NOF_DU_UES      = 1024,
  INVALID_DU_UE_INDEX = 1024
};

/// Maximum number of cells supported by DU (implementation-defined).
enum du_cell_index_t : uint16_t {
  MIN_DU_CELL_INDEX     = 0,
  MAX_DU_CELL_INDEX     = 15,
  MAX_NOF_DU_CELLS      = 16,
  INVALID_DU_CELL_INDEX = MAX_NOF_DU_CELLS
};

/// Convert integer to DU UE index type.
constexpr inline du_ue_index_t to_du_ue_index(std::underlying_type_t<du_ue_index_t> idx)
{
  return static_cast<du_ue_index_t>(idx);
}

constexpr inline bool is_du_ue_index_valid(du_ue_index_t ue_idx)
{
  return ue_idx < MAX_NOF_DU_UES;
}

/// Convert integer to DU cell index type.
inline du_cell_index_t to_du_cell_index(std::underlying_type_t<du_cell_index_t> idx)
{
  return static_cast<du_cell_index_t>(idx);
}

/// Maximum number of cells supported by a single UE.
enum ue_cell_index_t : uint16_t {
  PCELL_INDEX           = 0,
  MAX_UE_SCELL_INDEX    = 15,
  MAX_NOF_SCELL_INDEXES = 16,
  INVALID_UE_CELL_INDEX = MAX_NOF_SCELL_INDEXES
};

inline ue_cell_index_t to_ue_cell_index(std::underlying_type_t<ue_cell_index_t> idx)
{
  return static_cast<ue_cell_index_t>(idx);
}

/// \brief DU-specific index to group of cells that might be aggregated into a UE-specific CellGroup, if the UE
/// is CA-capable.
enum du_cell_group_index_t : uint16_t {
  MAX_DU_CELL_GROUPS          = du_cell_index_t::MAX_NOF_DU_CELLS,
  INVALID_DU_CELL_GROUP_INDEX = MAX_DU_CELL_GROUPS
};

} // namespace srsran
