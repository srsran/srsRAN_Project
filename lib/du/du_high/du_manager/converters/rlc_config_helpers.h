/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/f1ap/du/f1ap_du_ue_config.h"
#include "srsran/mac/mac_lc_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rb_id.h"
#include "srsran/rlc/rlc_config.h"
#include "srsran/rlc/rlc_factory.h"

namespace srsran {
namespace srs_du {

/// \brief Create configuration for RLC SRB entity.
rlc_entity_creation_message make_rlc_entity_creation_message(gnb_du_id_t                              du_id,
                                                             du_ue_index_t                            ue_index,
                                                             du_cell_index_t                          pcell_index,
                                                             du_ue_srb&                               bearer,
                                                             const rlc_config&                        rlc_cfg,
                                                             const du_manager_params::service_params& du_services,
                                                             rlc_tx_upper_layer_control_notifier&     rlc_rlf_notifier,
                                                             rlc_pcap&                                rlc_pcap);

/// \brief Create configuration for RLC DRB entity.
rlc_entity_creation_message make_rlc_entity_creation_message(gnb_du_id_t                              du_id,
                                                             du_ue_index_t                            ue_index,
                                                             du_cell_index_t                          pcell_index,
                                                             du_ue_drb&                               bearer,
                                                             const rlc_config&                        rlc_cfg,
                                                             const du_manager_params::service_params& du_services,
                                                             rlc_tx_upper_layer_control_notifier&     rlc_rlf_notifier,
                                                             rlc_metrics_notifier*                    rlc_metrics_notif,
                                                             rlc_pcap&                                rlc_pcap);

} // namespace srs_du
} // namespace srsran
