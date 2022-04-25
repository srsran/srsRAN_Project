
#ifndef SRSGNB_RAN_DU_UE_LIST_H
#define SRSGNB_RAN_DU_UE_LIST_H

#include "du_types.h"
#include "srsgnb/adt/slot_array.h"

namespace srsgnb {

/// \brief Representation of a list of UEs indexed by DU UE Index.
template <typename T>
using du_ue_list = slot_array<T, MAX_DU_NOF_UES>;

} // namespace srsgnb

#endif // SRSGNB_RAN_DU_UE_LIST_H
