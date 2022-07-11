/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_RLC_FACTORY_H
#define SRSGNB_RLC_RLC_FACTORY_H

#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/rlc/rlc_entity.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {

struct rlc_entity_creation_message {
  du_ue_index_t                        ue_index;
  lcid_t                               lcid;
  rlc_config                           config;
  rlc_rx_upper_layer_data_notifier*    upper_dn;
  rlc_tx_upper_layer_control_notifier* upper_cn;
  timer_manager*                       timers;
};

/// Creates an instance of a RLC bearer
std::unique_ptr<rlc_entity> create_rlc_entity(const rlc_entity_creation_message& msg);

} // namespace srsgnb

#endif // SRSGNB_RLC_RLC_FACTORY_H
