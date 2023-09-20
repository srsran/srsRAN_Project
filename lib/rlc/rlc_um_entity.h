/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "rlc_base_entity.h"
#include "rlc_rx_um_entity.h"
#include "rlc_tx_um_entity.h"

namespace srsran {

class rlc_um_entity : public rlc_base_entity
{
public:
  rlc_um_entity(du_ue_index_t                        ue_index_,
                rb_id_t                              rb_id_,
                const rlc_um_config&                 config,
                timer_duration                       metrics_period_,
                rlc_metrics_notifier*                rlc_metrics_notifier_,
                rlc_rx_upper_layer_data_notifier&    rx_upper_dn,
                rlc_tx_upper_layer_data_notifier&    tx_upper_dn,
                rlc_tx_upper_layer_control_notifier& tx_upper_cn,
                rlc_tx_lower_layer_notifier&         tx_lower_dn,
                timer_manager&                       timers,
                task_executor&                       pcell_executor,
                task_executor&                       ue_executor) :
    rlc_base_entity(ue_index_, rb_id_, metrics_period_, rlc_metrics_notifier_, timer_factory{timers, ue_executor})
  {
    tx = std::make_unique<rlc_tx_um_entity>(
        ue_index_, rb_id_, config.tx, tx_upper_dn, tx_upper_cn, tx_lower_dn, pcell_executor);
    rx = std::make_unique<rlc_rx_um_entity>(
        ue_index_, rb_id_, config.rx, rx_upper_dn, timer_factory{timers, ue_executor}, ue_executor);
  }
};

} // namespace srsran
