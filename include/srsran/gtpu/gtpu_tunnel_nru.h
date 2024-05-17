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

#include "srsran/gtpu/gtpu_tunnel_common_rx.h"
#include "srsran/gtpu/gtpu_tunnel_nru_tx.h"

namespace srsran {

/// Class used to interface with an GTP-U tunnel.
/// It will contain getters for the TX and RX entities interfaces.
/// TX and RX is considered from the perspective of the GTP-U.
class gtpu_tunnel_nru
{
public:
  gtpu_tunnel_nru()                                   = default;
  virtual ~gtpu_tunnel_nru()                          = default;
  gtpu_tunnel_nru(const gtpu_tunnel_nru&)             = delete;
  gtpu_tunnel_nru& operator=(const gtpu_tunnel_nru&)  = delete;
  gtpu_tunnel_nru(const gtpu_tunnel_nru&&)            = delete;
  gtpu_tunnel_nru& operator=(const gtpu_tunnel_nru&&) = delete;

  virtual gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() = 0;
  virtual gtpu_tunnel_nru_tx_lower_layer_interface*    get_tx_lower_layer_interface() = 0;
};

} // namespace srsran