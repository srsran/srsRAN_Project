/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
