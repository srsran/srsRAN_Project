
#ifndef SRSGNB_RAN_DU_TYPES_H
#define SRSGNB_RAN_DU_TYPES_H

#include <cstdint>
#include <type_traits>

namespace srsgnb {

/// Maximum number of UEs supported by DU (implementation-defined).
enum du_ue_index_t : uint16_t { MIN_DU_UE_INDEX = 0, MAX_DU_UE_INDEX = 1023, MAX_NOF_DU_UES = 1024 };

/// Maximum number of cells supported by DU (implementation-defined).
enum du_cell_index_t : uint16_t { MIN_DU_CELL_INDEX = 0, MAX_DU_CELL_INDEX = 15, MAX_NOF_DU_CELLS = 16 };

using du_bwp_id_t = uint16_t;

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

} // namespace srsgnb

#endif // SRSGNB_RAN_DU_TYPES_H
