/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/du/f1u_bearer.h"
#include "srsran/f1u/du/f1u_config.h"
#include "srsran/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsran/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsran/ran/rb_id.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

struct f1u_bearer_creation_message {
  uint32_t                 ue_index;
  drb_id_t                 drb_id;
  up_transport_layer_info  dl_tnl_info;
  f1u_config               config;
  f1u_rx_sdu_notifier*     rx_sdu_notifier;
  f1u_tx_pdu_notifier*     tx_pdu_notifier;
  timer_factory            timers;
  task_executor*           ue_executor;
  f1u_bearer_disconnector* disconnector;
};

/// \brief Creates an F1-U bearer for the DU.
std::unique_ptr<f1u_bearer> create_f1u_bearer(const f1u_bearer_creation_message& msg);

} // namespace srs_du
} // namespace srsran
