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

namespace srsran {

/// Class used to interface with an GTP-U path management entity.
/// It will contain getters for the RX entities interfaces.
/// TX and RX is considered from the perspective of the GTP-U.
class gtpu_echo
{
public:
  gtpu_echo()                             = default;
  virtual ~gtpu_echo()                    = default;
  gtpu_echo(const gtpu_echo&)             = delete;
  gtpu_echo& operator=(const gtpu_echo&)  = delete;
  gtpu_echo(const gtpu_echo&&)            = delete;
  gtpu_echo& operator=(const gtpu_echo&&) = delete;

  virtual gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() = 0;
};

} // namespace srsran
