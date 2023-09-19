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

#include "../du_ue/du_bearer.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/f1ap/du/f1ap_du_ue_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/rlc/rlc_config.h"
#include "srsran/rlc/rlc_factory.h"

namespace srsran {
namespace srs_du {

struct rlc_bearer_config {
  lcid_t             lcid;
  optional<drb_id_t> drb_id;
  rlc_config         rlc_cfg;

  bool operator==(const rlc_bearer_config& rhs) const
  {
    // TODO: Remaining fields
    return lcid == rhs.lcid and drb_id == rhs.drb_id and rlc_cfg.mode == rhs.rlc_cfg.mode;
  }
};

/// \brief Generates default SRB RLC configuration for SRBs other than SRB0.
rlc_config make_default_srb_rlc_config();

/// \brief Create configuration for RLC SRB entity.
rlc_entity_creation_message make_rlc_entity_creation_message(du_ue_index_t                            ue_index,
                                                             du_cell_index_t                          pcell_index,
                                                             du_ue_srb&                               bearer,
                                                             const du_manager_params::service_params& du_services,
                                                             rlc_tx_upper_layer_control_notifier&     rlc_rlf_notifier);

/// \brief Create configuration for RLC DRB entity.
rlc_entity_creation_message make_rlc_entity_creation_message(du_ue_index_t                            ue_index,
                                                             du_cell_index_t                          pcell_index,
                                                             du_ue_drb&                               bearer,
                                                             const du_manager_params::service_params& du_services,
                                                             rlc_tx_upper_layer_control_notifier&     rlc_rlf_notifier,
                                                             rlc_metrics_notifier* rlc_metrics_notif);

} // namespace srs_du
} // namespace srsran
