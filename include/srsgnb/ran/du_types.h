
#ifndef SRSGNB_DU_TYPES_H
#define SRSGNB_DU_TYPES_H

#include <cstdint>

namespace srsgnb {

using du_cell_index_t = uint16_t;
using du_ue_index_t   = uint16_t;

constexpr du_ue_index_t MAX_NOF_UES = 64;

} // namespace srsgnb

#endif // SRSGNB_DU_TYPES_H
