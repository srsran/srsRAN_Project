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

#include "f1u_bearer.h"
#include "f1u_rx_sdu_notifier.h"
#include "f1u_tx_pdu_notifier.h"
#include "srsran/f1u/du/f1u_config.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_du {

struct f1u_bearer_creation_message {
  uint32_t                ue_index;
  drb_id_t                drb_id;
  up_transport_layer_info dl_tnl_info;
  f1u_config              config;
  f1u_rx_sdu_notifier*    rx_sdu_notifier;
  f1u_tx_pdu_notifier*    tx_pdu_notifier;
  timer_factory           timers;
  task_executor*          ue_executor;
};

/// \brief Creates an F1-U bearer for the DU.
std::unique_ptr<f1u_bearer> create_f1u_bearer(const f1u_bearer_creation_message& msg);

} // namespace srs_du
} // namespace srsran
