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
namespace srs_cu_cp {

/// Maximum number of UEs supported by CU-CP (implementation-defined).
enum ue_index_t : uint16_t { MIN_UE_INDEX = 0, MAX_UE_INDEX = 1023, MAX_NOF_UES = 1024 };

/// Maximum number of DUs supported by CU-CP (implementation-defined).
enum du_index_t : uint16_t { MIN_DU_INDEX = 0, MAX_DU_INDEX = 1, MAX_NOF_DUS = 2 };

/// Maximum number of cells per DU supported by CU-CP (implementation-defined).
enum du_cell_index_t : uint16_t { MIN_DU_CELL_INDEX = 0, MAX_DU_CELL_INDEX = 15, MAX_NOF_DU_CELLS = 16 };

/// Convert integer to CU UE index type.
constexpr inline ue_index_t to_cu_ue_index(std::underlying_type_t<ue_index_t> idx)
{
  return static_cast<ue_index_t>(idx);
}

constexpr inline bool is_cu_ue_index_valid(ue_index_t ue_idx)
{
  return ue_idx < MAX_NOF_UES;
}

/// Convert integer to CU cell index type.
inline du_cell_index_t to_du_cell_index(std::underlying_type_t<du_cell_index_t> idx)
{
  return static_cast<du_cell_index_t>(idx);
}

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_RAN_CU_CP_TYPES_H
