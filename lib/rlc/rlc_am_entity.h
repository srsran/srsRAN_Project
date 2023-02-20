/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_base_entity.h"
#include "rlc_rx_am_entity.h"
#include "rlc_tx_am_entity.h"

namespace srsran {

class rlc_am_entity : public rlc_base_entity
{
public:
  rlc_am_entity(du_ue_index_t                        du_index,
                rb_id_t                              rb_id,
                const rlc_am_config&                 config,
                rlc_rx_upper_layer_data_notifier&    rx_upper_dn,
                rlc_tx_upper_layer_data_notifier&    tx_upper_dn,
                rlc_tx_upper_layer_control_notifier& tx_upper_cn,
                rlc_tx_lower_layer_notifier&         tx_lower_dn,
                timer_manager&                       timers,
                task_executor&                       pcell_executor,
                task_executor&                       ue_executor) :
    rlc_base_entity(du_index, rb_id)
  {
    // Create AM entities
    std::unique_ptr<rlc_tx_am_entity> tx_am = std::make_unique<rlc_tx_am_entity>(
        du_index, rb_id, config.tx, tx_upper_dn, tx_upper_cn, tx_lower_dn, timers, pcell_executor);
    std::unique_ptr<rlc_rx_am_entity> rx_am =
        std::make_unique<rlc_rx_am_entity>(du_index, rb_id, config.rx, rx_upper_dn, timers, ue_executor);

    // Tx/Rx interconnect
    tx_am->set_status_provider(rx_am.get());
    rx_am->set_status_handler(tx_am.get());
    rx_am->set_status_notifier(tx_am.get());

    // Store entities
    tx = std::move(tx_am);
    rx = std::move(rx_am);
  }
};

} // namespace srsran
