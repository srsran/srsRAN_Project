/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
  rlc_rx_tm_entity(uint32_t                          du_index,
                   du_ue_index_t                     ue_index,
                   rb_id_t                           rb_id,
                   const rlc_rx_tm_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                   bool                              metrics_enabled,
                   rlc_pcap&                         pcap_);

  void stop() final{
      // There are no timers to be stopped here.
  };

  // Interfaces for higher layers
  void handle_pdu(byte_buffer_slice buf) override;
};

} // namespace srsran
