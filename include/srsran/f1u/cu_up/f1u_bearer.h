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

#include "f1u_rx_pdu_handler.h"
#include "f1u_tx_sdu_handler.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {
namespace srs_cu_up {

constexpr uint32_t f1u_dl_notif_time_ms = 10;

class f1u_bearer_disconnector;

class f1u_bearer
{
public:
  f1u_bearer()          = default;
  virtual ~f1u_bearer() = default;

  f1u_bearer(const f1u_bearer&)            = delete;
  f1u_bearer(f1u_bearer&&)                 = delete;
  f1u_bearer& operator=(const f1u_bearer&) = delete;
  f1u_bearer& operator=(f1u_bearer&&)      = delete;

  virtual f1u_rx_pdu_handler& get_rx_pdu_handler() = 0;
  virtual f1u_tx_sdu_handler& get_tx_sdu_handler() = 0;
};

/// This class represents the interface through which a F1-U bearer disconnects from its gateway upon destruction
class f1u_bearer_disconnector
{
public:
  virtual ~f1u_bearer_disconnector() = default;

  virtual void disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info) = 0;
};

} // namespace srs_cu_up
} // namespace srsran
