
#ifndef SRSGNB_NR_ID_TYPES_H
#define SRSGNB_NR_ID_TYPES_H

#include <cstdint>

namespace srsgnb {

using du_cell_index_t = uint16_t;
using du_ue_index_t   = uint16_t;
using lcid_t          = uint16_t;
using rnti_t          = uint16_t;
using slot_point      = uint32_t;

constexpr du_ue_index_t MAX_NOF_UES  = 64;
constexpr rnti_t        INVALID_RNTI = 0;

} // namespace srsgnb

#endif // SRSGNB_NR_ID_TYPES_H
