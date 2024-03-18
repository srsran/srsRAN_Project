/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "du_ue_adapters.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/f1u/du/f1u_config.h"
#include "srsran/mac/mac_lc_config.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/qos/five_qi_qos_mapping.h"
#include "srsran/ran/qos/qos_info.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/rlc/rlc_config.h"
#include "srsran/rlc/rlc_entity.h"
#include <map>

namespace srsran {

class gtpu_teid_pool;

namespace srs_du {

struct du_manager_params;

/// \brief Connector of the MAC, RLC and F1 for a given DU UE SRB bearer.
struct du_srb_connector {
  mac_sdu_rx_adapter              mac_rx_sdu_notifier;
  mac_sdu_tx_adapter              mac_tx_sdu_notifier;
  rlc_rx_rrc_sdu_adapter          rlc_rx_sdu_notif;
  rlc_f1c_tx_data_notifier        rlc_tx_data_notif;
  rlc_tx_mac_buffer_state_updater rlc_tx_buffer_state_notif;
  f1c_rx_sdu_rlc_adapter          f1c_rx_sdu_notif;

  /// \brief Connect bearers of MAC, RLC and F1AP layers.
  void connect(du_ue_index_t                       ue_index,
               srb_id_t                            srb_id,
               f1c_bearer&                         f1_bearer,
               rlc_entity&                         rlc_bearer,
               mac_ue_control_information_handler& mac_ue_info_handler);

  /// \brief Disconnect DRB MAC, RLC and F1-U notifiers.
  void disconnect();
};

/// \brief Connector of the MAC, RLC and F1 for a given DU UE DRB bearer.
struct du_drb_connector {
  mac_sdu_rx_adapter              mac_rx_sdu_notifier;
  mac_sdu_tx_adapter              mac_tx_sdu_notifier;
  rlc_f1u_tx_sdu_adapter          rlc_rx_sdu_notif;
  rlc_f1u_tx_data_notifier        rlc_tx_data_notif;
  rlc_tx_mac_buffer_state_updater rlc_tx_buffer_state_notif;
  f1u_rx_rlc_sdu_adapter          f1u_rx_sdu_notif;

  /// \brief Connect MAC, RLC and F1AP layers if bearer is a DRB.
  void connect(du_ue_index_t                       ue_index,
               drb_id_t                            drb_id,
               lcid_t                              lcid,
               f1u_bearer&                         f1_bearer,
               rlc_entity&                         rlc_bearer,
               mac_ue_control_information_handler& mac_ue_info_handler);

  /// \brief Disconnect DRB MAC, RLC and F1-U notifiers.
  void disconnect();
};

/// \brief SRB instance in DU manager. It contains SRB configuration information, RLC entity and adapters between
/// layers.
struct du_ue_srb {
  srb_id_t                    srb_id;
  rlc_config                  rlc_cfg;
  mac_lc_config               mac_cfg;
  std::unique_ptr<rlc_entity> rlc_bearer;
  du_srb_connector            connector;

  lcid_t lcid() const { return srb_id_to_lcid(srb_id); }

  /// \brief Stops SRB by disconnecting MAC, RLC and F1-U notifiers and stopping the RLC timers.
  void stop();
};

/// \brief DRB instance in DU manager. It contains DRB configuration information, RLC entity and adapters between
/// layers.
struct du_ue_drb {
  drb_id_t                                                      drb_id;
  lcid_t                                                        lcid;
  std::vector<up_transport_layer_info>                          uluptnl_info_list;
  std::vector<up_transport_layer_info>                          dluptnl_info_list;
  rlc_config                                                    rlc_cfg;
  std::unique_ptr<rlc_entity>                                   rlc_bearer;
  mac_lc_config                                                 mac_cfg;
  f1u_config                                                    f1u_cfg;
  std::unique_ptr<f1u_bearer, std::function<void(f1u_bearer*)>> drb_f1u;
  du_drb_connector                                              connector;
  /// QoS characteristics to be met by the DRB.
  qos_characteristics qos_info;
  /// QoS information present only for GBR QoS flows.
  optional<gbr_qos_info_t> gbr_qos_info;

  /// \brief Stops DRB by disconnecting MAC, RLC and F1-U notifiers and stopping the RLC timers.
  void stop();
};

/// \brief Creates a DRB instance for the whole DU.
std::unique_ptr<du_ue_drb> create_drb(du_ue_index_t                        ue_index,
                                      du_cell_index_t                      pcell_index,
                                      drb_id_t                             drb_id,
                                      lcid_t                               lcid,
                                      const rlc_config&                    rlc_cfg,
                                      const mac_lc_config&                 mac_cfg,
                                      const f1u_config&                    f1u_cfg,
                                      span<const up_transport_layer_info>  uluptnl_info_list,
                                      gtpu_teid_pool&                      teid_pool,
                                      const du_manager_params&             du_params,
                                      rlc_tx_upper_layer_control_notifier& rlc_rlf_notifier,
                                      const qos_characteristics&           qos_info,
                                      optional<gbr_qos_info_t>             gbr_qos_info);

} // namespace srs_du
} // namespace srsran
