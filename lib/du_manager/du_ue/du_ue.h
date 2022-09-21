/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../adapters/du_bearer_adapter.h"
#include "cell_group_config.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac_sdu_handler.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rlc/rlc_entity.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_du {

struct du_bearer {
  lcid_t   lcid;
  drb_id_t drbid; // Valid if DRB.

  std::unique_ptr<rlc_entity> rlc_bearer;
  du_bearer_adapter           bearer_connector;
};

struct du_ue {
  explicit du_ue(du_ue_index_t ue_index_, du_cell_index_t pcell_index_, rnti_t rnti_) :
    ue_index(ue_index_), rnti(rnti_), pcell_index(pcell_index_)
  {
  }

  const du_ue_index_t                     ue_index;
  rnti_t                                  rnti;
  du_cell_index_t                         pcell_index;
  slot_array<du_bearer, MAX_NOF_RB_LCIDS> bearers;

  std::vector<cell_group_config> cells;
};

} // namespace srs_du
} // namespace srsgnb
