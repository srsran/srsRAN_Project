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

#include "srsgnb/gtpu/gtpu_tunnel_rx.h"
#include "srsgnb/gtpu/gtpu_tunnel_tx.h"

namespace srsgnb {

/// Class used to interface with an GTP-U tunnel.
/// It will contain getters for the TX and RX entities interfaces.
/// TX and RX is considered from the perspective of the GTP-U.
class gtpu_tunnel
{
public:
  gtpu_tunnel()                               = default;
  virtual ~gtpu_tunnel()                      = default;
  gtpu_tunnel(const gtpu_tunnel&)             = delete;
  gtpu_tunnel& operator=(const gtpu_tunnel&)  = delete;
  gtpu_tunnel(const gtpu_tunnel&&)            = delete;
  gtpu_tunnel& operator=(const gtpu_tunnel&&) = delete;

  virtual gtpu_tunnel_rx_upper_layer_interface* get_rx_upper_layer_interface() = 0;
  virtual gtpu_tunnel_tx_lower_layer_interface* get_tx_lower_layer_interface() = 0;
};

} // namespace srsgnb
