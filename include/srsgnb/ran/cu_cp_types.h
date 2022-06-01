/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_CU_CP_TYPES_H
#define SRSGNB_RAN_CU_CP_TYPES_H

#include <cstdint>
#include <type_traits>

namespace srsgnb {

/// Maximum number of UEs supported by CU-CP (implementation-defined).
enum cu_cp_ue_index_t : uint16_t { MIN_CU_CP_UE_INDEX = 0, MAX_CU_CP_UE_INDEX = 1023, MAX_NOF_CU_CP_UES = 1024 };

/// Maximum number of cells supported by CU-CP (implementation-defined).
enum cu_cp_cell_index_t : uint16_t { MIN_CU_CP_CELL_INDEX = 0, MAX_CU_CP_CELL_INDEX = 15, MAX_NOF_CU_CP_CELLS = 16 };

/// Convert integer to CU UE index type.
constexpr inline cu_cp_ue_index_t to_cu_ue_index(std::underlying_type_t<cu_cp_ue_index_t> idx)
{
  return static_cast<cu_cp_ue_index_t>(idx);
}

constexpr inline bool is_cu_ue_index_valid(cu_cp_ue_index_t ue_idx)
{
  return ue_idx < MAX_NOF_CU_CP_UES;
}

/// Convert integer to CU cell index type.
inline cu_cp_cell_index_t to_cu_cell_index(std::underlying_type_t<cu_cp_ue_index_t> idx)
{
  return static_cast<cu_cp_cell_index_t>(idx);
}

} // namespace srsgnb

#endif // SRSGNB_RAN_CU_CP_TYPES_H
