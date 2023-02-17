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

#include "rlc_rx_entity.h"

namespace srsgnb {

class rlc_rx_tm_entity : public rlc_rx_entity
{
public:
  rlc_rx_tm_entity(du_ue_index_t du_index, rb_id_t rb_id, rlc_rx_upper_layer_data_notifier& upper_dn_);

  // Interfaces for higher layers
  void handle_pdu(byte_buffer_slice buf) override;
};

} // namespace srsgnb
