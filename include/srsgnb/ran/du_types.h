
#ifndef SRSGNB_RAN_DU_TYPES_H
#define SRSGNB_RAN_DU_TYPES_H

#include <cstdint>

namespace srsgnb {

using du_cell_index_t = uint16_t;
using du_ue_index_t   = uint16_t;
using du_bwp_id_t     = uint16_t;

/// Maximum number of UEs supported by DU (implementation-defined)
constexpr du_ue_index_t   MAX_DU_NOF_UES   = 1024;
constexpr du_cell_index_t MAX_DU_NOF_CELLS = 16;

inline bool is_du_ue_index_valid(du_ue_index_t ue_idx)
{
  return ue_idx < MAX_DU_NOF_UES;
}

} // namespace srsgnb

#endif // SRSGNB_RAN_DU_TYPES_H
