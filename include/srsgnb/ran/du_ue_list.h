/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_DU_UE_LIST_H
#define SRSGNB_RAN_DU_UE_LIST_H

#include "du_types.h"
#include "srsgnb/adt/slot_array.h"

namespace srsgnb {

/// \brief Representation of a list of UEs indexed by DU UE Index.
template <typename T>
using du_ue_list = slot_array<T, MAX_NOF_DU_UES>;

} // namespace srsgnb

#endif // SRSGNB_RAN_DU_UE_LIST_H
