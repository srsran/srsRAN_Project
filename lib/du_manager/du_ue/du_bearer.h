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

#include "../adapters/f1_adapters.h"
#include "../adapters/mac_adapters.h"
#include "../adapters/rlc_adapters.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/rlc/rlc_entity.h"

namespace srsgnb {
namespace srs_du {

/// \brief Connector of the MAC, RLC and F1 for a given DU UE bearer.
struct du_bearer_adapter {
  mac_sdu_rx_adapter              mac_rx_notif;
  mac_sdu_tx_adapter              mac_tx_notif;
  rlc_rx_rrc_sdu_adapter          rlc_rx_f1c_sdu_notif;
  rlc_f1u_tx_sdu_adapter          rlc_rx_f1u_sdu_notif;
  rlc_tx_data_notifier            rlc_tx_data_notif;
  rlc_tx_control_notifier         rlc_tx_ctrl_notif;
  rlc_tx_mac_buffer_state_updater rlc_tx_buffer_state_notif;
  f1c_rx_sdu_rlc_adapter          f1c_rx_sdu_notif;
  f1u_rx_rlc_sdu_adapter          f1u_rx_sdu_notif; // TODO: Rename to f1u_rx_rlc_tx_sdu_notif

  /// \brief Connect MAC, RLC and F1AP layers if bearer is an SRB.
  void connect(du_ue_index_t                       ue_index,
               srb_id_t                            srb_id,
               f1c_bearer&                         f1_bearer,
               rlc_entity&                         rlc_bearer,
               mac_ue_control_information_handler& mac_ue_info_handler);

  /// \brief Connect MAC, RLC and F1AP layers if bearer is a DRB.
  void connect(du_ue_index_t                       ue_index,
               drb_id_t                            drb_id,
               lcid_t                              lcid,
               f1u_bearer&                         f1_bearer,
               rlc_entity&                         rlc_bearer,
               mac_ue_control_information_handler& mac_ue_info_handler);
};

/// \brief Configuration of a DU bearer.
struct du_bearer {
  lcid_t             lcid;
  optional<drb_id_t> drbid; // Valid if DRB.
  rlc_config         rlc_cfg;

  std::unique_ptr<rlc_entity> rlc_bearer;
  du_bearer_adapter           bearer_connector;
};

} // namespace srs_du
} // namespace srsgnb