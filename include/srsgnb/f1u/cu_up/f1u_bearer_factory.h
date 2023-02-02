/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "f1u_bearer.h"
#include "f1u_rx_delivery_notifier.h"
#include "f1u_rx_sdu_notifier.h"
#include "f1u_tx_pdu_notifier.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {
namespace srs_cu_up {

/// \brief Creates an F1-U bearer for the CU-UP.
std::unique_ptr<f1u_bearer> create_f1u_bearer(uint32_t                  ue_index,
                                              drb_id_t                  drb_id,
                                              f1u_tx_pdu_notifier&      tx_pdu_notifier,
                                              f1u_rx_delivery_notifier& rx_delivery_notifier,
                                              f1u_rx_sdu_notifier&      rx_sdu_notifier,
                                              f1u_bearer_origin&        origin,
                                              uint32_t                  ul_teid);

} // namespace srs_cu_up
} // namespace srsgnb
