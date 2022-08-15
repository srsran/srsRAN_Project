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

#include "rlc_base_entity.h"
#include "rlc_rx_am_entity.h"
#include "rlc_tx_am_entity.h"

namespace srsgnb {

class rlc_am_entity : public rlc_base_entity
{
public:
  rlc_am_entity(du_ue_index_t                        du_index,
                lcid_t                               lcid,
                const rlc_am_config&                 config,
                rlc_rx_upper_layer_data_notifier&    rx_upper_dn,
                rlc_tx_upper_layer_data_notifier&    tx_upper_dn,
                rlc_tx_upper_layer_control_notifier& tx_upper_cn,
                rlc_tx_buffer_state_update_notifier& buffer_state_notifier,
                timer_manager&                       timers) :
    rlc_base_entity(du_index, lcid)
  {
    srsgnb_assert(config.tx != nullptr && config.rx != nullptr,
                  "Failed to create RLC AM. Config missing:{}{}",
                  config.tx == nullptr ? " TX" : "",
                  config.rx == nullptr ? " RX" : "");

    // Create AM entities
    std::unique_ptr<rlc_tx_am_entity> tx_am = std::make_unique<rlc_tx_am_entity>(
        du_index, lcid, *config.tx, tx_upper_dn, tx_upper_cn, buffer_state_notifier, timers);
    logger.log_info(
        "RLC TX AM configured: sn_field_length={}, t_poll_retx={}, max_retx_thresh={}, poll_pdu={}, poll_byte={}",
        config.tx->sn_field_length,
        config.tx->t_poll_retx,
        config.tx->max_retx_thresh,
        config.tx->poll_pdu,
        config.tx->poll_byte);

    std::unique_ptr<rlc_rx_am_entity> rx_am =
        std::make_unique<rlc_rx_am_entity>(du_index, lcid, *config.rx, rx_upper_dn, timers);
    logger.log_info("RLC RX AM configured: sn_field_length={}, t_reassembly={}, t_status_prohibit={}",
                    config.rx->sn_field_length,
                    config.rx->t_reassembly,
                    config.rx->t_status_prohibit);

    // Tx/Rx interconnect
    tx_am->set_status_provider(rx_am.get());
    rx_am->set_status_handler(tx_am.get());

    // Store entities
    tx = std::move(tx_am);
    rx = std::move(rx_am);
  }
};

} // namespace srsgnb
