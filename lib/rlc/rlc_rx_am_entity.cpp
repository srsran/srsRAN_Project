/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_rx_am_entity.h"

using namespace srsgnb;

rlc_rx_am_entity::rlc_rx_am_entity(du_ue_index_t                     du_index,
                                   lcid_t                            lcid,
                                   const rlc_rx_am_config&           config,
                                   rlc_rx_upper_layer_data_notifier& upper_dn,
                                   timer_manager&                    timers) :
  rlc_rx_entity(du_index, lcid, upper_dn), cfg(config), mod(cardinality(to_number(cfg.sn_field_length)))
{
}
