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

#include "cell_group_config.h"
#include "du_bearer.h"
#include "du_ue_resource_manager.h"
#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/mac/mac_sdu_handler.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rlc/rlc_entity.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_du {

struct du_ue {
  explicit du_ue(du_ue_index_t               ue_index_,
                 du_cell_index_t             pcell_index_,
                 rnti_t                      rnti_,
                 du_ue_resource_configurator resources_) :
    ue_index(ue_index_), rnti(rnti_), pcell_index(pcell_index_), resources(std::move(resources_))
  {
  }

  const du_ue_index_t         ue_index;
  rnti_t                      rnti;
  du_cell_index_t             pcell_index;
  du_ue_bearer_manager        bearers;
  unique_timer                activity_timer;
  du_ue_resource_configurator resources;
};

} // namespace srs_du
} // namespace srsgnb
