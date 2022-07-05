/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UE_CONTEXT_H
#define SRSGNB_UE_CONTEXT_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {
namespace srs_cu_cp {

struct ue_context {
  ue_index_t      ue_index;
  du_cell_index_t pcell_index;
  rnti_t          c_rnti;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_UE_CONTEXT_H
