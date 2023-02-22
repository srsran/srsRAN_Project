/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/gtpu/gtpu_tunnel_rx.h"
#include "srsran/gtpu/gtpu_tunnel_tx.h"

namespace srsran {

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

} // namespace srsran
