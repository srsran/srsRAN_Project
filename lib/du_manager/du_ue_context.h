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

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac_sdu_handler.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rlc/rlc_entity.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_du {

struct du_logical_channel_context {
  lcid_t                                               lcid;
  std::unique_ptr<rlc_rx_upper_layer_data_notifier>    rlc_rx_ul_sdu_notif;
  std::unique_ptr<rlc_tx_upper_layer_data_notifier>    rlc_tx_ul_data_notif;
  std::unique_ptr<rlc_tx_upper_layer_control_notifier> rlc_tx_ul_ctrl_notif;
  std::unique_ptr<rlc_tx_lower_layer_notifier>         rlc_tx_ll_data_notif;
  std::unique_ptr<rlc_entity>                          rlc_bearer;
  std::unique_ptr<mac_sdu_rx_notifier>                 mac_rx_notifier;
  std::unique_ptr<mac_sdu_tx_builder>                  mac_tx_notifier;
};

struct du_ue_context {
  du_ue_index_t                           ue_index;
  rnti_t                                  rnti;
  du_cell_index_t                         pcell_index;
  slot_vector<du_logical_channel_context> bearers;
};

} // namespace srs_du
} // namespace srsgnb
