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
  // Config storage
  const rlc_rx_tm_config cfg;

  pcap_rlc_pdu_context pcap_context;

public:
  rlc_rx_tm_entity(gnb_du_id_t                       gnb_du_id,
                   du_ue_index_t                     ue_index,
                   rb_id_t                           rb_id,
                   const rlc_rx_tm_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                   bool                              metrics_enabled,
                   rlc_pcap&                         pcap_);

  void stop() final
  {
    // There are no timers to be stopped here.
  }

  // Interfaces for higher layers
  void handle_pdu(byte_buffer_slice buf) override;
};

} // namespace srsran
