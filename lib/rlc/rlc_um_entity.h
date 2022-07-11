/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_RLC_RLC_UM_ENTITY_H
#define SRSGNB_LIB_RLC_RLC_UM_ENTITY_H

#include "rlc_base_entity.h"
#include "rlc_rx_um_entity.h"
#include "rlc_tx_um_entity.h"

namespace srsgnb {

class rlc_um_entity : public rlc_base_entity
{
public:
  rlc_um_entity(du_ue_index_t                        du_index,
                lcid_t                               lcid,
                const rlc_um_config&                 config,
                rlc_rx_upper_layer_data_notifier&    upper_dn,
                rlc_tx_upper_layer_control_notifier& upper_cn,
                timer_manager&                       timers) :
    rlc_base_entity(du_index, lcid)
  {
    if (config.tx != nullptr) {
      tx = std::unique_ptr<rlc_tx_entity>(new rlc_tx_um_entity(du_index, lcid, *config.tx, upper_cn));
      logger.log_info("RLC TX UM configured: sn_field_length={}", to_number(config.tx->sn_field_length));
    } else {
      logger.log_info("Configured RLC UM without TX entity");
    }
    if (config.rx != nullptr) {
      rx = std::unique_ptr<rlc_rx_entity>(new rlc_rx_um_entity(du_index, lcid, *config.rx, upper_dn, timers));
      logger.log_info("RLC RX UM configured: sn_field_length={}, t_reassembly_ms={}",
                      to_number(config.rx->sn_field_length),
                      config.rx->t_reassembly_ms);
    } else {
      logger.log_info("Configured RLC UM without RX entity");
    }
  }
};

} // namespace srsgnb

#endif // SRSGNB_LIB_RLC_RLC_UM_ENTITY_H
