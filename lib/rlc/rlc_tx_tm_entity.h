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

#include "rlc_sdu_queue.h"
#include "rlc_tx_entity.h"

namespace srsgnb {

class rlc_tx_tm_entity : public rlc_tx_entity
{
private:
  rlc_sdu_queue sdu_queue;

public:
  rlc_tx_tm_entity(du_ue_index_t                        du_index,
                   rb_id_t                              rb_id,
                   rlc_tx_upper_layer_data_notifier&    upper_dn_,
                   rlc_tx_upper_layer_control_notifier& upper_cn_,
                   rlc_tx_lower_layer_notifier&         lower_dn_);

  // Interfaces for higher layers
  void handle_sdu(rlc_sdu sdu) override;
  void discard_sdu(uint32_t pdcp_sn) override;

  // Interfaces for lower layers
  byte_buffer_slice_chain pull_pdu(uint32_t grant_len) override;

  uint32_t get_buffer_state() override;

  void handle_buffer_state_update();
};

} // namespace srsgnb
