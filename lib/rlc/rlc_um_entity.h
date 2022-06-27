/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_UM_BEARER_H
#define SRSGNB_RLC_UM_BEARER_H

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
                rlc_tx_upper_layer_control_notifier& upper_cn) :
    rlc_base_entity(du_index, lcid)
  {
    tx = std::unique_ptr<rlc_tx_entity>(new rlc_tx_um_entity(du_index, lcid, config, upper_cn));
    rx = std::unique_ptr<rlc_rx_entity>(new rlc_rx_um_entity(du_index, lcid, config, upper_dn));
  }
};

} // namespace srsgnb

#endif
