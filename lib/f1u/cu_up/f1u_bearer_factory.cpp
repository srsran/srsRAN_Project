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

#include "srsran/f1u/cu_up/f1u_bearer_factory.h"
#include "f1u_bearer_impl.h"

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<f1u_bearer> srsran::srs_cu_up::create_f1u_bearer(uint32_t                       ue_index,
                                                                 drb_id_t                       drb_id,
                                                                 const up_transport_layer_info& ul_up_tnl_info,
                                                                 f1u_tx_pdu_notifier&           tx_pdu_notifier,
                                                                 f1u_rx_delivery_notifier&      rx_delivery_notifier,
                                                                 f1u_rx_sdu_notifier&           rx_sdu_notifier,
                                                                 timer_factory                  timers,
                                                                 f1u_bearer_disconnector&       disconnector)
{
  auto bearer = std::make_unique<f1u_bearer_impl>(
      ue_index, drb_id, ul_up_tnl_info, tx_pdu_notifier, rx_delivery_notifier, rx_sdu_notifier, timers, disconnector);
  return bearer;
}
