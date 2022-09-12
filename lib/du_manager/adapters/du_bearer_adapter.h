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

#include "f1_adapters.h"
#include "mac_adapters.h"
#include "rlc_adapters.h"

namespace srsgnb {
namespace srs_du {

struct du_bearer_adapter {
  mac_sdu_rx_adapter              mac_rx_notif;
  mac_sdu_tx_adapter              mac_tx_notif;
  rlc_rx_rrc_sdu_adapter          rlc_rx_sdu_notif;
  rlc_tx_data_notifier            rlc_tx_data_notif;
  rlc_tx_control_notifier         rlc_tx_ctrl_notif;
  rlc_tx_mac_buffer_state_updater rlc_tx_buffer_state_notif;
  f1_tx_rlc_pdu_adapter           f1_tx_pdu_notif;
};

} // namespace srs_du
} // namespace srsgnb
