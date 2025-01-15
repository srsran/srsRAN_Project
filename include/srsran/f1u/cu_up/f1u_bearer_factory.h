/*
 *
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/cu_up/f1u_bearer.h"
#include "srsran/f1u/cu_up/f1u_config.h"
#include "srsran/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsran/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsran/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsran/ran/rb_id.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_cu_up {

/// \brief Creates an F1-U bearer for the CU-UP.
std::unique_ptr<f1u_bearer> create_f1u_bearer(uint32_t                       ue_index,
                                              drb_id_t                       drb_id,
                                              const up_transport_layer_info& ul_up_tnl_info,
                                              const f1u_config&              config,
                                              f1u_tx_pdu_notifier&           tx_pdu_notifier,
                                              f1u_rx_delivery_notifier&      rx_delivery_notifier,
                                              f1u_rx_sdu_notifier&           rx_sdu_notifier,
                                              timer_factory                  ue_dl_timer_factory,
                                              unique_timer&                  ue_inactivity_timer,
                                              task_executor&                 ul_exec);

} // namespace srs_cu_up
} // namespace srsran
