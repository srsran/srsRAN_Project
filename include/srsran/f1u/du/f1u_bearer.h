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

#include "f1u_rx_pdu_handler.h"
#include "f1u_tx_delivery_handler.h"
#include "f1u_tx_sdu_handler.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {
namespace srs_du {

class f1u_bearer
{
public:
  virtual ~f1u_bearer() = default;

  virtual f1u_rx_pdu_handler&      get_rx_pdu_handler()      = 0;
  virtual f1u_tx_delivery_handler& get_tx_delivery_handler() = 0;
  virtual f1u_tx_sdu_handler&      get_tx_sdu_handler()      = 0;

  virtual void stop() = 0;
};

/// This class represents the interface through which a F1-U bearer disconnects from its gateway upon removal
class f1u_bearer_disconnector
{
public:
  virtual ~f1u_bearer_disconnector() = default;

  virtual void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) = 0;
};

} // namespace srs_du
} // namespace srsran
