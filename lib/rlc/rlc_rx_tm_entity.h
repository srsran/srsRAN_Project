/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rlc_rx_entity.h"

namespace srsran {

class rlc_rx_tm_entity : public rlc_rx_entity
{
private:
  pcap_rlc_pdu_context pcap_context;

public:
  rlc_rx_tm_entity(uint32_t                          du_index,
                   du_ue_index_t                     ue_index,
                   rb_id_t                           rb_id,
                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                   bool                              metrics_enabled,
                   rlc_pcap&                         pcap_);

  // Interfaces for higher layers
  void handle_pdu(byte_buffer_slice buf) override;
};

} // namespace srsran
