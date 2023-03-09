/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ran_resource_management/cell_group_config.h"
#include "../ran_resource_management/du_ran_resource_manager.h"
#include "du_bearer.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace srs_du {

struct du_ue {
  explicit du_ue(du_ue_index_t                ue_index_,
                 du_cell_index_t              pcell_index_,
                 rnti_t                       rnti_,
                 ue_ran_resource_configurator resources_) :
    ue_index(ue_index_), rnti(rnti_), pcell_index(pcell_index_), resources(std::move(resources_))
  {
  }

  const du_ue_index_t          ue_index;
  rnti_t                       rnti;
  du_cell_index_t              pcell_index;
  du_ue_bearer_manager         bearers;
  unique_timer2                activity_timer;
  ue_ran_resource_configurator resources;
};

} // namespace srs_du
} // namespace srsran
