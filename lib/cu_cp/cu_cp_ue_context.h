/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_UE_CONTEXT_H
#define SRSGNB_CU_CP_UE_CONTEXT_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"

namespace srsgnb {

struct cu_cp_ue_context {
  cu_cp_ue_index_t ue_index;
  cu_cp_du_index_t      du_index;
  cu_cp_du_cell_index_t pcell_index;
};

} // namespace srsgnb

#endif // SRSGNB_CU_CP_UE_CONTEXT_H
