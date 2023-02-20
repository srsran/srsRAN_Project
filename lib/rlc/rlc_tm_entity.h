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
#include "rlc_rx_tm_entity.h"
#include "rlc_tx_tm_entity.h"

namespace srsran {

class rlc_tm_entity : public rlc_base_entity
{
public:
  rlc_tm_entity(du_ue_index_t                        ue_index,
                rb_id_t                              rb_id,
                rlc_rx_upper_layer_data_notifier&    rx_upper_dn,
                rlc_tx_upper_layer_data_notifier&    tx_upper_dn,
                rlc_tx_upper_layer_control_notifier& tx_upper_cn,
                rlc_tx_lower_layer_notifier&         tx_lower_dn) :
    rlc_base_entity(ue_index, rb_id)
  {
    tx = std::unique_ptr<rlc_tx_entity>(new rlc_tx_tm_entity(ue_index, rb_id, tx_upper_dn, tx_upper_cn, tx_lower_dn));
    rx = std::unique_ptr<rlc_rx_entity>(new rlc_rx_tm_entity(ue_index, rb_id, rx_upper_dn));
  }
};

} // namespace srsran
