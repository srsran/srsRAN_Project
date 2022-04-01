
#ifndef SRSGNB_RNTI_MAP_H
#define SRSGNB_RNTI_MAP_H

#include "du_types.h"
#include "rnti.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include <array>
#include <memory>

namespace srsgnb {

/// \brief Representation of a list of UEs indexed by RNTI.
template <typename T>
using rnti_map = circular_map<rnti_t, T, MAX_NOF_UES>;

/// \brief Representation of a list of UEs indexed by DU UE Index.
template <typename T>
using du_ue_list = slot_array<T, MAX_NOF_UES>;

} // namespace srsgnb

#endif // SRSGNB_RNTI_MAP_H
