/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../ran_resource_management/cell_group_config.h"
#include "../ran_resource_management/du_ran_resource_manager.h"
#include "du_ue_bearer_manager.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace srs_du {

/// \brief This class holds the context of an UE in the DU.
struct du_ue {
  explicit du_ue(du_ue_index_t                                        ue_index_,
                 du_cell_index_t                                      pcell_index_,
                 rnti_t                                               rnti_,
                 std::unique_ptr<mac_ue_radio_link_notifier>          mac_rlf_notifier_,
                 std::unique_ptr<rlc_tx_upper_layer_control_notifier> rlc_rlf_notifier_,
                 ue_ran_resource_configurator                         resources_) :
    ue_index(ue_index_),
    rnti(rnti_),
    pcell_index(pcell_index_),
    mac_rlf_notifier(std::move(mac_rlf_notifier_)),
    rlc_rlf_notifier(std::move(rlc_rlf_notifier_)),
    resources(std::move(resources_))
  {
  }

  const du_ue_index_t ue_index;
  rnti_t              rnti;
  du_cell_index_t     pcell_index;

  std::unique_ptr<mac_ue_radio_link_notifier>          mac_rlf_notifier;
  std::unique_ptr<rlc_tx_upper_layer_control_notifier> rlc_rlf_notifier;

  du_ue_bearer_manager         bearers;
  ue_ran_resource_configurator resources;
  bool                         reestablishment_pending = false;
};

} // namespace srs_du
} // namespace srsran
