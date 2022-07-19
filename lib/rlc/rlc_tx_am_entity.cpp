/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_tx_am_entity.h"

using namespace srsgnb;

rlc_tx_am_entity::rlc_tx_am_entity(du_ue_index_t                        du_index,
                                   lcid_t                               lcid,
                                   const rlc_tx_am_config&              config,
                                   rlc_tx_upper_layer_control_notifier& upper_cn,
                                   rlc_tx_buffer_state_update_notifier& buffer_state_notif,
                                   timer_manager&                       timers) :
  rlc_tx_entity(du_index, lcid, upper_cn, buffer_state_notif),
  cfg(config),
  mod(cardinality(to_number(cfg.sn_field_length)))
{
}
